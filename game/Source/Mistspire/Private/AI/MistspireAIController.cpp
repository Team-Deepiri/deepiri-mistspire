#include "MistspireAIController.h"
#include "MistspireStateMachine.h"
#include "GameFramework/Pawn.h"

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
		UtilityEvaluator = UMistspireUtilityEvaluator::MakeClimberEvaluator(this);
	}

	UtilityEvaluator->SetInput(TEXT("Oxygen"), WorldState.Oxygen01);
	UtilityEvaluator->SetInput(TEXT("Stamina"), WorldState.Stamina01);
	UtilityEvaluator->SetInput(TEXT("Warmth"), WorldState.Warmth01);
	UtilityEvaluator->SetInput(TEXT("Exposure"), WorldState.bExposed ? 1.f : 0.f);
	UtilityEvaluator->SetInput(TEXT("Altitude"), FMath::Clamp(WorldState.AltitudeCm / 1200000.f, 0.f, 1.f));

	return UtilityEvaluator->EvaluateBest();
}

bool AMistspireAIController::PlanGOAP(const FMistspireGOAPState& Goal, TArray<FMistspireGOAPAction>& OutPlan, int32 MaxDepth)
{
	FMistspireGOAPState Start;
	Start.Facts.FindOrAdd(TEXT("HasOxygen")) = WorldState.bHasOxygen;
	Start.Facts.FindOrAdd(TEXT("HasStamina")) = WorldState.bHasStamina;
	Start.Facts.FindOrAdd(TEXT("Climbing")) = false;
	Start.Facts.FindOrAdd(TEXT("IsSheltered")) = false;
	Start.Facts.FindOrAdd(TEXT("Exposed")) = WorldState.bExposed;
	Start.Facts.FindOrAdd(TEXT("CanisterNearby")) = true;
	Start.Facts.FindOrAdd(TEXT("ShelterKnown")) = true;
	Start.Values.FindOrAdd(TEXT("Altitude")) = WorldState.AltitudeCm;
	Start.Values.FindOrAdd(TEXT("Oxygen")) = WorldState.Oxygen01 * 100.f;
	Start.Values.FindOrAdd(TEXT("Stamina")) = WorldState.Stamina01 * 100.f;

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
	Possessed->SetActorLocation(Possessed->GetActorLocation() + Velocity * DeltaTime);
	Possessed->SetActorRotation(Velocity.GetSafeNormal2D().Rotation());
}