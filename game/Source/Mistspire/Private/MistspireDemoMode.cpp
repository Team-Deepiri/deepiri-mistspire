#include "MistspireDemoMode.h"
#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireDemoClimbScaffold.h"
#include "MistspireDialogueSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireGameState.h"
#include "MistspireVRPawn.h"
#include "MistspireVisualEnhancementSubsystem.h"
#include "MistspireDemoSpireLayout.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "AI/MistspireWanderingGhost.h"
#include "Async/Async.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Systems/MistspireBiomeSubsystem.h"

static TAutoConsoleVariable<int32> CVarMistspireDemoMode(
	TEXT("mistspire.DemoMode"),
	0,
	TEXT("1 = demo presentation: HUD on, welcome dialogue, wandering ghosts."),
	ECVF_Default);

namespace
{
	bool IsPlayWorld(const UWorld* World)
	{
		return World
			&& (World->WorldType == EWorldType::Game
				|| World->WorldType == EWorldType::PIE
				|| World->WorldType == EWorldType::GamePreview);
	}

	UWorld* ResolvePlayWorld()
	{
		if (GWorld && IsPlayWorld(GWorld))
		{
			return GWorld;
		}
		if (!GEngine)
		{
			return nullptr;
		}
		for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
		{
			if (UWorld* World = Ctx.World())
			{
				if (IsPlayWorld(World))
				{
					return World;
				}
			}
		}
		return nullptr;
	}

	void EnsureDemoRuntimeOnGameThread()
	{
		if (UWorld* World = ResolvePlayWorld())
		{
			MistspireDemoMode::EnsureDemoRuntime(World);
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Mistspire DemoMode: no play world yet — markers/scaffold apply on next PIE StartPlay or ApplyDemoPresentation."));
		}
	}
}

bool MistspireDemoMode::IsEnabled()
{
	if (CVarMistspireDemoMode.GetValueOnGameThread() > 0)
	{
		return true;
	}
	return FParse::Param(FCommandLine::Get(), TEXT("demoworld"))
		|| FParse::Param(FCommandLine::Get(), TEXT("mistspiredemo"));
}

void MistspireDemoMode::EnsureDemoRuntime(UWorld* World)
{
	if (!IsPlayWorld(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire DemoMode: EnsureDemoRuntime refused — not a game/PIE world."));
		return;
	}

	if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		Atlas->SeedProductionWorld();
		Atlas->SpawnAuthoredWorldMarkers();
	}

	AMistspireDemoClimbScaffold::EnsureInWorld(World);
	ApplyPresentation(World);
	UE_LOG(LogTemp, Log, TEXT("Mistspire DemoMode: EnsureDemoRuntime complete (atlas markers + scaffold + presentation)."));
}

namespace
{
	void OnDemoModeCVarChanged(IConsoleVariable* Variable)
	{
		if (!Variable || Variable->GetInt() <= 0)
		{
			return;
		}
		// Mid-session enable: StartPlay already skipped markers when IsEnabled was false.
		if (IsInGameThread())
		{
			EnsureDemoRuntimeOnGameThread();
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, []()
			{
				EnsureDemoRuntimeOnGameThread();
			});
		}
	}

	struct FMistspireDemoModeCVarHook
	{
		FMistspireDemoModeCVarHook()
		{
			CVarMistspireDemoMode->SetOnChangedCallback(
				FConsoleVariableDelegate::CreateStatic(&OnDemoModeCVarChanged));
		}
	};
	static FMistspireDemoModeCVarHook GMistspireDemoModeCVarHook;
}

void MistspireDemoMode::ApplyPresentation(UWorld* World)
{
	if (!IsPlayWorld(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire DemoMode: refused — not a game/PIE world."));
		return;
	}

	UMistspireAltitudeDebugSubsystem::SetHudEnabled(true);

	if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
	{
		Env->ForceWeather(EMistspireWeatherType::Clear, 180.f);
	}

	if (UMistspireDialogueSubsystem* Dialogue = World->GetSubsystem<UMistspireDialogueSubsystem>())
	{
		Dialogue->Speak(TEXT("companion_greeting"));
	}

	APawn* Pawn = nullptr;
	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		Pawn = PC->GetPawn();
	}

	const FVector Base = Pawn ? Pawn->GetActorLocation() : FVector::ZeroVector;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	int32 DemoGhosts = 0;
	TArray<AMistspireWanderingGhost*> ExistingDemoGhosts;
	for (TActorIterator<AMistspireWanderingGhost> It(World); It; ++It)
	{
		if (It->bSpawnedForDemo)
		{
			ExistingDemoGhosts.Add(*It);
			++DemoGhosts;
		}
	}

	const FVector Offsets[] = {
		FVector(500.f, 350.f, 900.f),
		FVector(-450.f, 500.f, 1100.f)
	};

	if (DemoGhosts == 0)
	{
		for (const FVector& Offset : Offsets)
		{
			if (AMistspireWanderingGhost* Ghost = World->SpawnActor<AMistspireWanderingGhost>(
				Base + Offset, FRotator::ZeroRotator, Params))
			{
				Ghost->bSpawnedForDemo = true;
			}
		}
	}
	else
	{
		for (int32 i = 0; i < ExistingDemoGhosts.Num() && i < UE_ARRAY_COUNT(Offsets); ++i)
		{
			if (ExistingDemoGhosts[i])
			{
				ExistingDemoGhosts[i]->SetActorLocation(Base + Offsets[i], false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}

	if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
	{
		GS->BroadcastSocialAchievement(TEXT("Demo mode — tour biomes with mistspire.DemoTour."));
	}

	UE_LOG(LogTemp, Log, TEXT("Mistspire DemoMode: presentation applied (HUD, dialogue, ghosts)."));
}

bool MistspireDemoMode::TeleportToBiomeIndex(UWorld* World, int32 BiomeIndex, bool bForceVisuals)
{
	if (!IsPlayWorld(World))
	{
		return false;
	}

	if (BiomeIndex == -1)
	{
		if (UMistspireVisualEnhancementSubsystem* Vis = World->GetSubsystem<UMistspireVisualEnhancementSubsystem>())
		{
			Vis->ClearForcedBiome();
		}
		UE_LOG(LogTemp, Log, TEXT("Mistspire DemoTour: cleared forced biome visuals."));
		return true;
	}

	if (BiomeIndex < 0 || BiomeIndex >= MistspireDemoSpire::StationCount)
	{
		return false;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn)
	{
		return false;
	}

	const FVector Loc = MistspireDemoSpire::GetTourLandingLocation(BiomeIndex);

	if (AMistspireVRPawn* MistPawn = Cast<AMistspireVRPawn>(Pawn))
	{
		MistPawn->ResetMotionForDebugTeleport();
		MistPawn->ApplyTeleport(Loc);
	}
	else
	{
		Pawn->SetActorLocation(Loc, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (UMistspireVisualEnhancementSubsystem* Vis = World->GetSubsystem<UMistspireVisualEnhancementSubsystem>())
	{
		if (bForceVisuals)
		{
			const EMistspireBiomeType Biome = static_cast<EMistspireBiomeType>(BiomeIndex + 1);
			Vis->ForceBiomeVisuals(Biome);
		}
		else
		{
			Vis->ClearForcedBiome();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Mistspire DemoTour: biome index %d -> %s (%.0f, %.0f, %.0f)"),
		BiomeIndex, MistspireDemoSpire::BiomeNames[BiomeIndex], Loc.X, Loc.Y, Loc.Z);
	return true;
}
