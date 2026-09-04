#include "MistspireGameMode.h"
#include "MistspireGameState.h"
#include "MistspireVRPawn.h"
#include "MistspirePlayerState.h"
#include "MistspireSummitRegistry.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireProgressSubsystem.h"
#include "MistspireCompanionSubsystem.h"
#include "MistspireGhostClimberSubsystem.h"
#include "MistspireAmbienceSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireDialogueSubsystem.h"
#include "MistspireEntitySubsystem.h"
#include "MistspireObservationRecorder.h"
#include "MistspireLeaderboardService.h"
#include "AI/MistspireAIController.h"

AMistspireGameMode::AMistspireGameMode()
{
	DefaultPawnClass = AMistspireVRPawn::StaticClass();
	GameStateClass = AMistspireGameState::StaticClass();
	PlayerStateClass = AMistspirePlayerState::StaticClass();

	// Support for 30+ player massively vertical multiplayer
	bPauseable = false;
	bStartPlayersAsSpectators = false;
}

void AMistspireGameMode::StartPlay()
{
	Super::StartPlay();
	SeedDefaultSummits();
	SeedWorldAtlas();

	if (UWorld* World = GetWorld())
	{
		World->GetSubsystem<UMistspireAltitudeDebugSubsystem>();

		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
			{
				Progress->LoadProgress();
				Progress->ApplyLoadedProgressToWorld(World);
			}
		}

		World->GetSubsystem<UMistspireCompanionSubsystem>();
		World->GetSubsystem<UMistspireGhostClimberSubsystem>();
		World->GetSubsystem<UMistspireAmbienceSubsystem>();
		World->GetSubsystem<UMistspireDialogueSubsystem>();
		World->GetSubsystem<UMistspireEntitySubsystem>();
		World->GetSubsystem<UMistspireObservationRecorder>();

		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UMistspireLeaderboardService* Leaderboard = GI->GetSubsystem<UMistspireLeaderboardService>())
			{
				Leaderboard->RefreshLeaderboard();
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Mistspire: climb higher. mistspire.SaveProgress | SetWeather | RefillSurvival"));
		
		if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
		{
			GS->BroadcastSocialAchievement(TEXT("Welcome to Mistspire — climb higher."));
		}
	}
}

void AMistspireGameMode::SeedDefaultSummits()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>();
	if (!Registry)
	{
		return;
	}

	// World locations (cm) — move markers in Main_WP after landscape sculpt
	Registry->RegisterSummit(TEXT("summit_valley_gate"), FVector(0.f, 0.f, 20000.f), 20000.f);
	Registry->RegisterSummit(TEXT("summit_mesa_crown"), FVector(500000.f, 0.f, 150000.f), 150000.f);
	Registry->RegisterSummit(TEXT("summit_cloud_garden"), FVector(250000.f, 250000.f, 400000.f), 400000.f);
	Registry->RegisterSummit(TEXT("summit_obelisk_prime"), FVector(0.f, 500000.f, 600000.f), 600000.f);
	Registry->RegisterSummit(TEXT("summit_orbital_needle"), FVector(0.f, 0.f, 800000.f), 800000.f);
	Registry->RegisterSummit(TEXT("summit_spire_cathedral"), FVector(-300000.f, 400000.f, 550000.f), 550000.f);
	Registry->RegisterSummit(TEXT("summit_rift_observatory"), FVector(400000.f, -200000.f, 700000.f), 700000.f);
	Registry->RegisterSummit(TEXT("summit_ember_crown"), FVector(-150000.f, -350000.f, 350000.f), 350000.f);
}

void AMistspireGameMode::SeedWorldAtlas()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		Atlas->SeedProductionWorld();
		Atlas->SpawnAuthoredWorldMarkers();
	}
}
