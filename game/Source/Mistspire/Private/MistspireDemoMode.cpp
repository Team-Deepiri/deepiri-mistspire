#include "MistspireDemoMode.h"
#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireDialogueSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireGameState.h"
#include "MistspireVRPawn.h"
#include "MistspireVisualEnhancementSubsystem.h"
#include "AI/MistspireWanderingGhost.h"
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
	/** Mid-band altitudes (cm) aligned with BiomeFromAltitude in MistspireEnvironmentSubsystem. */
	constexpr float GDemoBiomeMidCm[] = {
		50000.f,   // Mist 0–1 km
		200000.f,  // Arid 1–3 km
		400000.f,  // Forest 3–5 km
		600000.f,  // Ember 5–7 km
		800000.f,  // Crystal 7–9 km
		1050000.f, // Void 9–12 km
		1300000.f, // Tundra 12–14 km
		1500000.f, // Aether 14–16 km
		1700000.f, // Sanctum 16–18 km
		1900000.f  // Pinnacle 18–20 km
	};

	constexpr int32 GDemoBiomeCount = UE_ARRAY_COUNT(GDemoBiomeMidCm);

	bool IsPlayWorld(const UWorld* World)
	{
		return World
			&& (World->WorldType == EWorldType::Game
				|| World->WorldType == EWorldType::PIE
				|| World->WorldType == EWorldType::GamePreview);
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

	if (BiomeIndex < 0 || BiomeIndex >= GDemoBiomeCount)
	{
		return false;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn)
	{
		return false;
	}

	const float TargetZ = GDemoBiomeMidCm[BiomeIndex];
	// Offset XY so we do not land exactly on summit markers at origin.
	const FVector Loc(15000.f, 15000.f, TargetZ);

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

	UE_LOG(LogTemp, Log, TEXT("Mistspire DemoTour: biome index %d -> Z=%.0f cm"), BiomeIndex, TargetZ);
	return true;
}
