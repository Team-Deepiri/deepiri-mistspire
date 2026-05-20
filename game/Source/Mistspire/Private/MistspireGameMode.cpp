#include "MistspireGameMode.h"
#include "MistspireGameState.h"
#include "MistspireVRPawn.h"
#include "MistspireSummitRegistry.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireAltitudeDebugSubsystem.h"

AMistspireGameMode::AMistspireGameMode()
{
	DefaultPawnClass = AMistspireVRPawn::StaticClass();
	GameStateClass = AMistspireGameState::StaticClass();
}

void AMistspireGameMode::StartPlay()
{
	Super::StartPlay();
	SeedDefaultSummits();

	if (UWorld* World = GetWorld())
	{
		World->GetSubsystem<UMistspireAltitudeDebugSubsystem>();
		UE_LOG(LogTemp, Log, TEXT("Mistspire: climb higher. Console: mistspire.AltitudeStats, mistspire.TeleportUp"));
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
}
