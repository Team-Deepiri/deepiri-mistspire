#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireVisualEnhancementSubsystem.h"
#include "MistspireProgressSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireAudioSubsystem.h"
#include "MistspireLog.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"
#if WITH_EDITOR
#include "Editor.h"
#include "Engine/Level.h"
#include "Engine/LevelScriptActor.h"
#include "FileHelpers.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "UObject/UObjectIterator.h"
#endif

static void MistspireAltitudeStats(const TArray<FString>& Args)
{
	if (UWorld* World = GWorld)
	{
		if (UMistspireAltitudeDebugSubsystem* Dbg = World->GetSubsystem<UMistspireAltitudeDebugSubsystem>())
		{
			Dbg->LogAltitudeStats();
		}
	}
}

static void MistspireTeleportUp(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}

	float DeltaCm = 5000.f;
	if (Args.Num() > 0)
	{
		DeltaCm = FCString::Atof(*Args[0]);
	}

	if (APawn* Pawn = GWorld->GetFirstPlayerController() ? GWorld->GetFirstPlayerController()->GetPawn() : nullptr)
	{
		Pawn->AddActorWorldOffset(FVector(0.f, 0.f, DeltaCm), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

static void MistspireSetWeather(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}
	UMistspireEnvironmentSubsystem* Env = GWorld->GetSubsystem<UMistspireEnvironmentSubsystem>();
	if (!Env)
	{
		return;
	}

	int32 Index = 0;
	if (Args.Num() > 0)
	{
		Index = FCString::Atoi(*Args[0]);
	}
	Index = FMath::Clamp(Index, 0, 3);
	Env->ForceWeather(static_cast<EMistspireWeatherType>(Index), 120.f);
}

static void MistspireRefillSurvival(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(GWorld->GetFirstPlayerController()->GetPawn()))
	{
		Pawn->ApplyShelterRefill(100.f, 100.f, 1.f);
	}
}

static FAutoConsoleCommand CmdMistspireAltitudeStats(
	TEXT("mistspire.AltitudeStats"),
	TEXT("Log current and personal-best altitude."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireAltitudeStats));

static FAutoConsoleCommand CmdMistspireTeleportUp(
	TEXT("mistspire.TeleportUp"),
	TEXT("Teleport player up Z (cm). Usage: mistspire.TeleportUp 5000"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireTeleportUp));

static FAutoConsoleCommand CmdMistspireSetWeather(
	TEXT("mistspire.SetWeather"),
	TEXT("Force weather: 0=Clear 1=MistStorm 2=Electric 3=ZenithGlow"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireSetWeather));

static FAutoConsoleCommand CmdMistspireRefillSurvival(
	TEXT("mistspire.RefillSurvival"),
	TEXT("Refill stamina and oxygen (debug)."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireRefillSurvival));

static void MistspireToggleSettings(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	APlayerController* PC = GWorld->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(PC->GetPawn()))
	{
		if (!Pawn->IsNonVRMode())
		{
			UE_LOG(LogTemp, Warning, TEXT("Mistspire: ToggleSettings is non-VR only."));
			return;
		}
		if (!Pawn->HasGameplayStarted())
		{
			Pawn->StartGameplay();
		}
		Pawn->ToggleSettingsMenu();
	}
}

static FAutoConsoleCommand CmdMistspireToggleSettings(
	TEXT("mistspire.ToggleSettings"),
	TEXT("Toggle the non-VR settings menu (use in PIE — Esc stops play in the editor)."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireToggleSettings));

static void MistspireSaveProgress(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UGameInstance* GI = GWorld->GetGameInstance())
	{
		if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
		{
			Progress->CaptureProgressFromWorld(GWorld);
			UE_LOG(LogTemp, Log, TEXT("Mistspire: progress saved."));
		}
	}
}

static void MistspireLoadProgress(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UGameInstance* GI = GWorld->GetGameInstance())
	{
		if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
		{
			Progress->LoadProgress();
			Progress->ApplyLoadedProgressToWorld(GWorld);
			UE_LOG(LogTemp, Log, TEXT("Mistspire: progress loaded."));
		}
	}
}

static FAutoConsoleCommand CmdMistspireSaveProgress(
	TEXT("mistspire.SaveProgress"),
	TEXT("Save personal best altitude and summits to disk."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireSaveProgress));

static FAutoConsoleCommand CmdMistspireLoadProgress(
	TEXT("mistspire.LoadProgress"),
	TEXT("Load saved progress into the current world."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireLoadProgress));

static void MistspireTeleportDistrict(const TArray<FString>& Args)
{
	if (!GWorld || Args.Num() < 1)
	{
		return;
	}
	const int32 Index = FMath::Clamp(FCString::Atoi(*Args[0]), 0, 11);
	if (UMistspireWorldAtlasSubsystem* Atlas = GWorld->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		const TArray<FMistspireDistrictEntry>& Districts = Atlas->GetDistricts();
		if (Districts.IsValidIndex(Index))
		{
			if (APawn* Pawn = GWorld->GetFirstPlayerController()->GetPawn())
			{
				const FVector Target = Districts[Index].BoundsCenter + FVector(0, 0, 25000.f);
				Pawn->SetActorLocation(Target, false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}

static void MistspireExitInterior(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(GWorld->GetFirstPlayerController()->GetPawn()))
	{
		if (UMistspireInteriorSubsystem* Interior = GWorld->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			Interior->ExitBuilding(Pawn);
		}
	}
}

static void MistspireRespawnWorldMarkers(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UMistspireWorldAtlasSubsystem* Atlas = GWorld->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		Atlas->SpawnAuthoredWorldMarkers();
	}
}

static FAutoConsoleCommand CmdMistspireTeleportDistrict(
	TEXT("mistspire.TeleportDistrict"),
	TEXT("Teleport to district index 0-11. Usage: mistspire.TeleportDistrict 0"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireTeleportDistrict));

static FAutoConsoleCommand CmdMistspireExitInterior(
	TEXT("mistspire.ExitInterior"),
	TEXT("Force exit current building interior."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireExitInterior));

static FAutoConsoleCommand CmdMistspireRespawnWorldMarkers(
	TEXT("mistspire.RespawnWorldMarkers"),
	TEXT("Respawn building doors and POI markers from atlas."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireRespawnWorldMarkers));

static void MistspireShowAltitudeHUD(const TArray<FString>& Args)
{
	if (!GWorld) return;
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(GWorld->GetFirstPlayerController()->GetPawn()))
	{
		uint8 Show = 1;
		if (Args.Num() > 0) Show = FMath::Clamp(FCString::Atoi(*Args[0]), 0, 1);
		if (Pawn->AltimeterText) Pawn->AltimeterText->SetHiddenInGame(Show == 0);
	}
}

static void RegisterMistspireAltitudeHudCommands()
{
	IConsoleManager& CM = IConsoleManager::Get();

	// Do not register mistspire.ShowAltitudeHUD here — it is a TAutoConsoleVariable<int32>
	// in MistspireAltitudeDebugSubsystem.cpp. A command with the same name fatals on load
	// when static init order registers the command first.

	if (!CM.FindConsoleObject(TEXT("mistspire.ShowWristAltimeter")))
	{
		CM.RegisterConsoleCommand(
			TEXT("mistspire.ShowWristAltimeter"),
			TEXT("Toggle wrist altimeter text visibility: 0=hide, 1=show."),
			FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireShowAltitudeHUD));
	}
}

struct FMistspireRegisterAltitudeHudCommands
{
	FMistspireRegisterAltitudeHudCommands() { RegisterMistspireAltitudeHudCommands(); }
};
static FMistspireRegisterAltitudeHudCommands GRegisterMistspireAltitudeHudCommands;

static void MistspireAudioDebug(const TArray<FString>&)
{
	if (!GWorld) return;
	if (UMistspireAudioSubsystem* Audio = GWorld->GetSubsystem<UMistspireAudioSubsystem>())
	{
		Audio->DebugAudioStats();
	}
}

static FAutoConsoleCommand CmdMistspireAudioDebug(
	TEXT("mistspire.DebugAudioStats"),
	TEXT("Log all audio bus states (volume, pitch, mute, filter)."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireAudioDebug));

static void MistspireVisualDebug(const TArray<FString>&)
{
	if (!GWorld) return;
	if (UMistspireVisualEnhancementSubsystem* Vis = GWorld->GetSubsystem<UMistspireVisualEnhancementSubsystem>())
	{
		FMistspireBiomeVisuals V = Vis->GetCurrentVisuals();
		UE_LOG(LogMistspire, Log, TEXT("--- Visual Debug ---"));
		UE_LOG(LogMistspire, Log, TEXT("Biome: %d"), (int32)Vis->GetCurrentBiome());
		UE_LOG(LogMistspire, Log, TEXT("Bloom: %.2f (threshold %.2f)"), V.BloomIntensity, V.BloomThreshold);
		UE_LOG(LogMistspire, Log, TEXT("AO: %.2f | FogDensity: %.2f | FogColor: %.2f %.2f %.2f"),
			V.AmbientOcclusionIntensity, V.VolumetricFogDensity,
			V.VolumetricFogColor.R, V.VolumetricFogColor.G, V.VolumetricFogColor.B);
		UE_LOG(LogMistspire, Log, TEXT("ColorTemp: %.0fK | Sat: %.2f | Contrast: %.2f | Gamma: %.2f"),
			V.ColorTemperature, V.Saturation, V.Contrast, V.Gamma);
	}
}

static FAutoConsoleCommand CmdMistspireVisualDebug(
	TEXT("mistspire.VisualDebug"),
	TEXT("Log current visual enhancement state."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireVisualDebug));

static void MistspireForceBiomeVisuals(const TArray<FString>& Args)
{
	if (!GWorld) return;
	if (UMistspireVisualEnhancementSubsystem* Vis = GWorld->GetSubsystem<UMistspireVisualEnhancementSubsystem>())
	{
		if (Args.Num() < 1)
		{
			Vis->ClearForcedBiome();
			UE_LOG(LogMistspire, Log, TEXT("Cleared forced biome visuals."));
			return;
		}
		int32 Idx = FCString::Atoi(*Args[0]);
		if (Idx >= 0 && Idx <= 10)
		{
			Vis->ForceBiomeVisuals((EMistspireBiomeType)Idx);
			UE_LOG(LogMistspire, Log, TEXT("Forced biome visuals to %d"), Idx);
		}
	}
}

static FAutoConsoleCommand CmdMistspireForceBiomeVisuals(
	TEXT("mistspire.ForceBiomeVisuals"),
	TEXT("Force biome visual preset (0-10). No arg to clear."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireForceBiomeVisuals));

static void MistspireVisualIntensity(const TArray<FString>& Args)
{
	if (!GWorld) return;
	if (UMistspireVisualEnhancementSubsystem* Vis = GWorld->GetSubsystem<UMistspireVisualEnhancementSubsystem>())
	{
		float Val = 1.f;
		if (Args.Num() > 0) Val = FCString::Atof(*Args[0]);
		Vis->SetOverrideIntensity(Val);
		UE_LOG(LogMistspire, Log, TEXT("Visual override intensity set to %.2f"), Val);
	}
}

static FAutoConsoleCommand CmdMistspireVisualIntensity(
	TEXT("mistspire.VisualIntensity"),
	TEXT("Set visual effects intensity multiplier (0-2)."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireVisualIntensity));

#if WITH_EDITOR
/** Same repair as the editor toast "Map Corruption: Multiple Level Script Actors" → Repair Map. */
static void MistspireRepairLevelScriptActors(const TArray<FString>&)
{
	if (!GEditor)
	{
		UE_LOG(LogMistspire, Error, TEXT("mistspire.RepairLevelScriptActors: GEditor is null."));
		return;
	}

	UWorld* World = GEditor->GetEditorWorldContext(/*bEnsureIsGWorld=*/false).World();
	if (!World || World->WorldType != EWorldType::Editor)
	{
		UE_LOG(LogMistspire, Error,
			TEXT("mistspire.RepairLevelScriptActors: load AncientWorld in the editor viewport (not PIE)."));
		return;
	}

	ULevel* Level = World->PersistentLevel;
	if (!Level)
	{
		UE_LOG(LogMistspire, Error, TEXT("mistspire.RepairLevelScriptActors: no persistent level."));
		return;
	}

	ALevelScriptActor* Primary = Level->GetLevelScriptActor();
	if (!Primary)
	{
		for (TObjectIterator<ALevelScriptActor> It; It; ++It)
		{
			if (It->GetOuter() == Level)
			{
				Primary = *It;
				break;
			}
		}
	}

	if (!Primary)
	{
		UE_LOG(LogMistspire, Warning, TEXT("mistspire.RepairLevelScriptActors: no LevelScriptActor found."));
		return;
	}

	TArray<ALevelScriptActor*> Extras = Primary->FindSiblingLevelScriptActors();
	UE_LOG(LogMistspire, Log, TEXT("Primary LSA: %s | extras: %d"), *Primary->GetPathName(), Extras.Num());

	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if (!EditorActorSubsystem)
	{
		UE_LOG(LogMistspire, Error, TEXT("mistspire.RepairLevelScriptActors: EditorActorSubsystem missing."));
		return;
	}

	int32 Destroyed = 0;
	for (ALevelScriptActor* LSA : Extras)
	{
		if (!IsValid(LSA))
		{
			continue;
		}
		UE_LOG(LogMistspire, Log, TEXT("Deleting extra LevelScriptActor: %s"), *LSA->GetPathName());
		if (EditorActorSubsystem->DestroyActor(LSA))
		{
			++Destroyed;
		}
		else
		{
			LSA->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_AllowPackageLinkerMismatch);
			LSA->MarkAsGarbage();
			++Destroyed;
			UE_LOG(LogMistspire, Warning, TEXT("DestroyActor failed; marked garbage + moved to transient: %s"), *LSA->GetName());
		}
	}

	if (Destroyed <= 0)
	{
		UE_LOG(LogMistspire, Warning,
			TEXT("No extra LSAs removed. Reload the map and click Repair Map on the bottom-right toast within 10s."));
		return;
	}

	Level->MarkPackageDirty();
	World->MarkPackageDirty();
	const bool bSaved = FEditorFileUtils::SaveLevel(Level);
	UE_LOG(LogMistspire, Log,
		TEXT("Removed %d extra LevelScriptActor(s). SaveLevel=%s. Close and reopen AncientWorld; Ensure must not return."),
		Destroyed, bSaved ? TEXT("ok") : TEXT("FAILED — Ctrl+S the map manually"));
}

static FAutoConsoleCommand CmdMistspireRepairLevelScriptActors(
	TEXT("mistspire.RepairLevelScriptActors"),
	TEXT("Delete duplicate LevelScriptActors (AncientWorld cook Ensure). Editor only — not PIE."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireRepairLevelScriptActors));
#endif // WITH_EDITOR
