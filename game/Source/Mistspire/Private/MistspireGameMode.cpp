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
#include "TimerManager.h"

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

		const bool bNonVR = FMistspireInputMode::IsNonVRMode(World);
		FMistspireInputMode::ApplyRendererOverrides(bNonVR);

		if (bNonVR)
		{
			World->GetTimerManager().SetTimer(
				NonVRPlaygroundTimerHandle,
				this,
				&AMistspireGameMode::DeferredNonVRSetup,
				0.15f,
				false);
			// Do not ClientMessage the WASD hint — AHUD draws those at the bottom of the view
			// and the same string is shown via mistspire.ShowControls.
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

void AMistspireGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (!FMistspireInputMode::IsNonVRMode(GetWorld()) || !NewPlayer)
	{
		return;
	}

	if (APawn* Pawn = NewPlayer->GetPawn())
	{
		Pawn->SetActorLocation(ResolveNonVRSpawnLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AMistspireGameMode::DeferredNonVRSetup()
{
	UWorld* World = GetWorld();
	if (!World || !FMistspireInputMode::IsNonVRMode(World))
	{
		return;
	}

	EnsureNonVRPlayground();

	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->SetActorLocation(ResolveNonVRSpawnLocation(), false, nullptr, ETeleportType::TeleportPhysics);
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
	if (const APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		Params.AddIgnoredActor(Pawn);
	}
	return World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params) && Hit.bBlockingHit;
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

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!CubeMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire non-VR: could not load engine cube mesh."));
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto SpawnPlaygroundMesh = [&](UStaticMesh* Mesh, const FVector& Location, const FVector& Scale) -> AStaticMeshActor*
	{
		AStaticMeshActor* Actor = World->SpawnActorDeferred<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(), FTransform(Location), nullptr, nullptr,
			Params.SpawnCollisionHandlingOverride);
		if (!Actor)
		{
			return nullptr;
		}
		if (UStaticMeshComponent* MeshComp = Actor->GetStaticMeshComponent())
		{
			MeshComp->SetMobility(EComponentMobility::Movable);
			if (Mesh)
			{
				MeshComp->SetStaticMesh(Mesh);
			}
		}
		Actor->SetActorScale3D(Scale);
		UGameplayStatics::FinishSpawningActor(Actor, FTransform(Location));
		if (UStaticMeshComponent* MeshComp = Actor->GetStaticMeshComponent())
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
			MeshComp->SetMobility(EComponentMobility::Static);
		}
		return Actor;
	};

	// Engine cube is 100 cm; scale Z=0.4 => 40 cm thick. Center 20 cm below origin so the top sits at Z=0.
	if (SpawnPlaygroundMesh(CubeMesh, FVector(0.f, 0.f, -20.f), FVector(400.f, 400.f, 0.4f)))
	{
		// floor spawned
	}

	for (int32 Step = 0; Step < 10; ++Step)
	{
		const float X = 400.f * static_cast<float>(Step + 1);
		const float Y = 250.f * FMath::Sin(static_cast<float>(Step) * 0.65f);
		const float Z = 50.f + static_cast<float>(Step) * 120.f;
		if (SpawnPlaygroundMesh(CubeMesh, FVector(X, Y, Z), FVector(6.f, 6.f, 0.5f)))
		{
			// platform spawned
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Mistspire non-VR playground spawned (no ground under PlayerStart)."));
}
