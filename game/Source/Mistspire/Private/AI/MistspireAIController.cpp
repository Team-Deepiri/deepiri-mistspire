#include "AI/MistspireAIController.h"
#include "AI/MistspireStateMachine.h"
#include "MistspireVRPawn.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireOxygenCanister.h"
#include "MistspireWindCrystal.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

AMistspireAIController::AMistspireAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bSetControlRotationFromPawnOrientation = false;

	Steering = CreateDefaultSubobject<UMistspireSteeringComponent>(TEXT("Steering"));
	Steering->SetAutoActivate(true);

	StateMachine = CreateDefaultSubobject<UMistspireStateMachineComponent>(TEXT("StateMachine"));

	// Prefer MoveTo over manual steering when the pawn has a navigation mesh.
	ActionLibrary = UMistspireGOAPPlanner::BuildMistspireActionLibrary();
}

void AMistspireAIController::UpdateWorldState(const FMistspireAIWorldState& NewState)
{
	WorldState = NewState;
}

FMistspireUtilityDecision AMistspireAIController::RunUtilityDecision()
{
	if (!UtilityEvaluator)
	{
		UtilityEvaluator = UMistspireUtilityEvaluator::MakeClimberEvaluator(this, ClimberTuning);
	}

	UtilityEvaluator->SetInput(TEXT("Oxygen"), WorldState.Oxygen01);
	UtilityEvaluator->SetInput(TEXT("Stamina"), WorldState.Stamina01);
	UtilityEvaluator->SetInput(TEXT("Warmth"), WorldState.Warmth01);
	UtilityEvaluator->SetInput(TEXT("Exposure"), WorldState.bExposed ? 1.f : 0.f);
	UtilityEvaluator->SetInput(TEXT("Altitude"), FMath::Clamp(WorldState.AltitudeCm / 1200000.f, 0.f, 1.f));

	return UtilityEvaluator->EvaluateBest();
}

FMistspireGOAPState AMistspireAIController::BuildGOAPStartState(const FMistspireAIWorldState& WorldState)
{
	FMistspireGOAPState Start;
	Start.Facts.FindOrAdd(TEXT("HasOxygen")) = WorldState.bHasOxygen;
	Start.Facts.FindOrAdd(TEXT("HasStamina")) = WorldState.bHasStamina;
	Start.Facts.FindOrAdd(TEXT("Climbing")) = WorldState.bClimbing;
	Start.Facts.FindOrAdd(TEXT("IsSheltered")) = WorldState.bIsSheltered;
	Start.Facts.FindOrAdd(TEXT("Exposed")) = WorldState.bExposed;
	Start.Facts.FindOrAdd(TEXT("CanisterNearby")) = WorldState.bCanisterNearby;
	Start.Facts.FindOrAdd(TEXT("ShelterKnown")) = WorldState.bShelterKnown;
	Start.Facts.FindOrAdd(TEXT("HasCrystal")) = WorldState.bHasCrystal;
	Start.Values.FindOrAdd(TEXT("Altitude")) = WorldState.AltitudeCm;
	Start.Values.FindOrAdd(TEXT("Oxygen")) = WorldState.Oxygen01 * 100.f;
	Start.Values.FindOrAdd(TEXT("Stamina")) = WorldState.Stamina01 * 100.f;
	Start.Values.FindOrAdd(TEXT("Warmth")) = WorldState.Warmth01 * 100.f;
	return Start;
}

FMistspireAIWorldState AMistspireAIController::SnapshotFromPawn(const APawn* SourcePawn, const UWorld* World)
{
	FMistspireAIWorldState State;
	if (!SourcePawn)
	{
		return State;
	}

	State.AltitudeCm = SourcePawn->GetActorLocation().Z;
	if (const AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(SourcePawn))
	{
		State.Oxygen01 = VRPawn->GetOxygenPercent();
		State.Stamina01 = VRPawn->GetStaminaPercent();
		State.bHasOxygen = State.Oxygen01 > 0.1f;
		State.bHasStamina = State.Stamina01 > 0.1f;
		State.bClimbing = VRPawn->bIsClimbing;
		State.bWantsRest = State.Stamina01 < 0.25f;
	}

	if (World)
	{
		if (const UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			State.bIsSheltered = Interior->IsInsideInterior();
		}

		if (const UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			const EMistspireWeatherType Weather = Env->GetCurrentWeather();
			State.bExposed = !State.bIsSheltered
				&& (Weather == EMistspireWeatherType::ElectricTurmoil || Weather == EMistspireWeatherType::ZenithGlow);

			if (State.bIsSheltered)
			{
				State.Warmth01 = 1.f;
			}
			else if (Weather == EMistspireWeatherType::MistStorm)
			{
				State.Warmth01 = 0.5f;
			}
			else
			{
				State.Warmth01 = 0.75f;
			}
		}

		if (const UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
		{
			State.bShelterKnown = Atlas->GetBuildings().Num() > 0;
		}

		const FVector PawnLocation = SourcePawn->GetActorLocation();
		const float NearbyRadiusSq = FMath::Square(15000.f);
		State.bCanisterNearby = false;
		State.bHasCrystal = false;
		for (TActorIterator<AMistspireOxygenCanister> It(World); It; ++It)
		{
			if (FVector::DistSquared(PawnLocation, It->GetActorLocation()) <= NearbyRadiusSq)
			{
				State.bCanisterNearby = true;
				break;
			}
		}

		for (TActorIterator<AMistspireWindCrystal> It(World); It; ++It)
		{
			if (FVector::DistSquared(PawnLocation, It->GetActorLocation()) <= NearbyRadiusSq)
			{
				State.bHasCrystal = true;
				break;
			}
		}
	}

	return State;
}

bool AMistspireAIController::PlanGOAP(const FMistspireGOAPState& Goal, TArray<FMistspireGOAPAction>& OutPlan, int32 MaxDepth)
{
	const FMistspireGOAPState Start = BuildGOAPStartState(WorldState);
	return UMistspireGOAPPlanner::Plan(Start, Goal, ActionLibrary, OutPlan, MaxDepth);
}

void AMistspireAIController::MoveSteeredTo(const FVector& WorldLocation)
{
	if (!Steering)
	{
		return;
	}
	Steering->SetMode(EMistspireSteeringMode::Arrive);
	Steering->SetTarget(WorldLocation);
	bSteeringActive = true;
}

void AMistspireAIController::WanderAround(const FVector& Center)
{
	if (!Steering)
	{
		return;
	}
	Steering->SetMode(EMistspireSteeringMode::Wander);
	Steering->SetTarget(Center);
	bSteeringActive = true;
}

void AMistspireAIController::StopSteering()
{
	bSteeringActive = false;
	if (Steering)
	{
		Steering->SetMode(EMistspireSteeringMode::Seek);
	}
}

void AMistspireAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bSteeringActive || !Steering)
	{
		return;
	}

	APawn* Possessed = GetPawn();
	if (!Possessed)
	{
		return;
	}

	const FVector Velocity = Steering->ComputeDesiredVelocity(
		Possessed->GetActorLocation(), Possessed->GetVelocity(), DeltaTime);

	// Pawns with an engine movement component receive the steered velocity as
	// movement input so collision/nav-mesh aware movement keeps working. The
	// direct transform fallback is intended for free-flight spirit-type pawns
	// without a movement component; it ignores physics sweeps by design.
	if (UPawnMovementComponent* MovementComponent = Possessed->GetMovementComponent())
	{
		MovementComponent->AddInputVector(Velocity);
	}
	else
	{
		Possessed->SetActorLocation(Possessed->GetActorLocation() + Velocity * DeltaTime);
		Possessed->SetActorRotation(Velocity.GetSafeNormal2D().Rotation());
	}
}