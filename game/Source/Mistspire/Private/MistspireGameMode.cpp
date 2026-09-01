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
#include "MistspireInputMode.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AMistspireGameMode::AMistspireGameMode()
{
	DefaultPawnClass = AMistspireVRPawn::StaticClass();
	GameStateClass = AMistspireGameState::StaticClass();
	PlayerStateClass = AMistspirePlayerState::StaticClass();

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

		if (FMistspireInputMode::IsNonVRMode(World))
		{
			EnsureNonVRPlayground();

			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (APawn* Pawn = PC->GetPawn())
				{
					Pawn->SetActorLocation(ResolveNonVRSpawnLocation(), false, nullptr, ETeleportType::TeleportPhysics);
				}
			}

			if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
			{
				GS->BroadcastSocialAchievement(
					TEXT("Non-VR mode — WASD move, mouse look, Shift climb/sprint, F grapple, G glider, T teleport, E interact."));
			}
		}
		else if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
		{
			GS->BroadcastSocialAchievement(TEXT("Welcome to Mistspire — climb higher."));
		}

		UE_LOG(LogTemp, Log, TEXT("Mistspire: climb higher. mistspire.SaveProgress | SetWeather | RefillSurvival"));
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
		const bool bNonVR = FMistspireInputMode::IsNonVRMode(World);
		if (!bNonVR || bSpawnAtlasMarkersInNonVR)
		{
			Atlas->SpawnAuthoredWorldMarkers();
		}
	}
}

bool AMistspireGameMode::HasGroundUnderLocation(const FVector& Location) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector TraceStart = Location + FVector(0.f, 0.f, 50.f);
	const FVector TraceEnd = Location - FVector(0.f, 0.f, 5000.f);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NonVRGroundCheck), false);
	return World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params) && Hit.bBlockingHit;
}

FVector AMistspireGameMode::ResolveNonVRSpawnLocation() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return FVector(0.f, 0.f, 200.f);
	}

	if (AActor* Start = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass()))
	{
		return Start->GetActorLocation() + FVector(0.f, 0.f, 100.f);
	}

	return FVector(0.f, 0.f, 200.f);
}

void AMistspireGameMode::EnsureNonVRPlayground()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector SpawnLoc = ResolveNonVRSpawnLocation();
	if (HasGroundUnderLocation(SpawnLoc))
	{
		UE_LOG(LogTemp, Log, TEXT("Mistspire non-VR: using existing map geometry under spawn."));
		return;
	}

	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!PlaneMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire non-VR: could not load engine plane mesh."));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(FVector::ZeroVector, FRotator::ZeroRotator, Params))
	{
		if (UStaticMeshComponent* FloorMesh = Floor->GetStaticMeshComponent())
		{
			FloorMesh->SetStaticMesh(PlaneMesh);
			FloorMesh->SetMobility(EComponentMobility::Static);
		}
		Floor->SetActorScale3D(FVector(400.f, 400.f, 1.f));
	}

	if (CubeMesh)
	{
		for (int32 Step = 0; Step < 10; ++Step)
		{
			const float X = 400.f * static_cast<float>(Step + 1);
			const float Y = 250.f * FMath::Sin(static_cast<float>(Step) * 0.65f);
			const float Z = 50.f + static_cast<float>(Step) * 120.f;
			if (AStaticMeshActor* Platform = World->SpawnActor<AStaticMeshActor>(FVector(X, Y, Z), FRotator::ZeroRotator, Params))
			{
				if (UStaticMeshComponent* PlatformMesh = Platform->GetStaticMeshComponent())
				{
					PlatformMesh->SetStaticMesh(CubeMesh);
					PlatformMesh->SetMobility(EComponentMobility::Static);
				}
				Platform->SetActorScale3D(FVector(6.f, 6.f, 0.5f));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Mistspire non-VR playground spawned (no ground under PlayerStart)."));
}
