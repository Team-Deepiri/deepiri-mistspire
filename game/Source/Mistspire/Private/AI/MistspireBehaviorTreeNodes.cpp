#include "AI/MistspireBehaviorTreeNodes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Name.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "MistspireDialogueSubsystem.h"
#include "MistspireVRPawn.h"

namespace MistspireBTMoveToLocation
{
	static bool IsWithinAcceptanceRadius(const FVector& Location, const FVector& Target, float RadiusCm)
	{
		return FVector::DistSquared2D(Location, Target) <= FMath::Square(RadiusCm);
	}
}

// ---------------------------------------------------------------------------
// ClimbToAltitude
// ---------------------------------------------------------------------------

UMistspireBTTask_ClimbToAltitude::UMistspireBTTask_ClimbToAltitude()
{
	NodeName = TEXT("Mistspire: Climb To Altitude");
	bNotifyTaskFinished = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UMistspireBTTask_ClimbToAltitude::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard || !ClimbGoalKey.IsSet())
	{
		return EBTNodeResult::Failed;
	}
	const float GoalCm = Blackboard->GetValue<UBlackboardKeyType_Float>(ClimbGoalKey.GetSelectedKeyID());
	AActor* Actor = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Actor)
	{
		Actor = OwnerComp.GetOwner();
	}
	if (!Actor)
	{
		return EBTNodeResult::Failed;
	}
	UE_LOG(LogTemp, Log, TEXT("Mistspire BT: climbing to %.0f cm"), GoalCm);
	return EBTNodeResult::InProgress;
}

void UMistspireBTTask_ClimbToAltitude::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* Actor = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Actor)
	{
		Actor = OwnerComp.GetOwner();
	}
	if (!Blackboard || !Blackboard->IsValidKey(ClimbGoalKey.GetSelectedKeyID()) || !Actor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float GoalCm = Blackboard->GetValue<UBlackboardKeyType_Float>(ClimbGoalKey.GetSelectedKeyID());
	FVector Location = Actor->GetActorLocation();
	const float Delta = GoalCm - Location.Z;
	if (FMath::Abs(Delta) <= ToleranceCm)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	Location.Z += FMath::Clamp(Delta, -ClimbSpeedCmPerSec * DeltaSeconds, ClimbSpeedCmPerSec * DeltaSeconds);
	Actor->SetActorLocation(Location);
}

EBTNodeResult::Type UMistspireBTTask_ClimbToAltitude::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

// ---------------------------------------------------------------------------
// MoveToLocation
// ---------------------------------------------------------------------------

UMistspireBTTask_MoveToLocation::UMistspireBTTask_MoveToLocation()
{
	NodeName = TEXT("Mistspire: Move To Location");
	bNotifyTaskFinished = true;
	bNotifyTick = true;
}

uint16 UMistspireBTTask_MoveToLocation::GetInstanceMemorySize() const
{
	return sizeof(FBTMoveToLocationMemory);
}

EBTNodeResult::Type UMistspireBTTask_MoveToLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTMoveToLocationMemory* Memory = reinterpret_cast<FBTMoveToLocationMemory*>(NodeMemory);
	if (Memory)
	{
		Memory->bWasMoving = false;
		Memory->bMoveRequested = false;
		Memory->MoveRequestId = FAIRequestID::InvalidRequest;
	}

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Blackboard || !MoveTargetKey.IsSet() || !Controller || !Controller->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	const FVector Target = Blackboard->GetValue<UBlackboardKeyType_Vector>(MoveTargetKey.GetSelectedKeyID());
	if (Memory)
	{
		Memory->TargetLocation = Target;
	}

	const EPathFollowingRequestResult::Type Result = Controller->MoveToLocation(
		Target, AcceptanceRadiusCm, /*bStopOnOverlap=*/false, /*bUsePathfinding=*/true,
		/*bProjectDestinationToNavigation=*/false, /*bCanStrafe=*/true);

	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}

	if (Memory)
	{
		if (UPathFollowingComponent* PathFollowing = Controller->GetPathFollowingComponent())
		{
			Memory->MoveRequestId = PathFollowing->GetCurrentRequestId();
		}
		Memory->bMoveRequested = true;
	}

	return EBTNodeResult::InProgress;
}

void UMistspireBTTask_MoveToLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTMoveToLocationMemory* Memory = reinterpret_cast<FBTMoveToLocationMemory*>(NodeMemory);
	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	if (!Controller || !Pawn || !Memory)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (MistspireBTMoveToLocation::IsWithinAcceptanceRadius(
		Pawn->GetActorLocation(), Memory->TargetLocation, AcceptanceRadiusCm))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	UPathFollowingComponent* PathFollowing = Controller->GetPathFollowingComponent();
	if (Memory->bMoveRequested && PathFollowing
		&& Memory->MoveRequestId.IsValid()
		&& PathFollowing->GetCurrentRequestId() != Memory->MoveRequestId)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const EPathFollowingStatus::Type Status = Controller->GetMoveStatus();
	if (Status == EPathFollowingStatus::Moving)
	{
		Memory->bWasMoving = true;
		return;
	}

	if (Status == EPathFollowingStatus::Idle && Memory->bWasMoving)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (Status == EPathFollowingStatus::Paused || Status == EPathFollowingStatus::Waiting)
	{
		return;
	}

	if (Status == EPathFollowingStatus::Idle && !Memory->bWasMoving)
	{
		return;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

EBTNodeResult::Type UMistspireBTTask_MoveToLocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* Controller = OwnerComp.GetAIOwner())
	{
		Controller->StopMovement();
	}
	return EBTNodeResult::Aborted;
}

// ---------------------------------------------------------------------------
// RefillOxygen
// ---------------------------------------------------------------------------

EBTNodeResult::Type UMistspireBTTask_RefillOxygen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}

// ---------------------------------------------------------------------------
// PlayDialogueLine
// ---------------------------------------------------------------------------

UMistspireBTTask_PlayDialogueLine::UMistspireBTTask_PlayDialogueLine()
{
	NodeName = TEXT("Mistspire: Play Dialogue Line");
}

EBTNodeResult::Type UMistspireBTTask_PlayDialogueLine::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	UWorld* World = OwnerComp.GetWorld();
	if (!Blackboard || !DialogueLineKey.IsSet() || !World)
	{
		return EBTNodeResult::Failed;
	}

	const FName LineId = Blackboard->GetValue<UBlackboardKeyType_Name>(DialogueLineKey.GetSelectedKeyID());
	if (UMistspireDialogueSubsystem* Dialogue = World->GetSubsystem<UMistspireDialogueSubsystem>())
	{
		Dialogue->Speak(LineId);
	}
	return EBTNodeResult::Succeeded;
}

// ---------------------------------------------------------------------------
// IsLowOxygen
// ---------------------------------------------------------------------------

UMistspireBTCondition_IsLowOxygen::UMistspireBTCondition_IsLowOxygen()
{
	NodeName = TEXT("Mistspire: Is Low Oxygen");
}

bool UMistspireBTCondition_IsLowOxygen::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	float Oxygen01 = 0.f;
	if (Blackboard && OxygenPercentKey.IsSet())
	{
		Oxygen01 = Blackboard->GetValue<UBlackboardKeyType_Float>(OxygenPercentKey.GetSelectedKeyID());
	}
	else if (AAIController* Controller = OwnerComp.GetAIOwner())
	{
		if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Controller->GetPawn()))
		{
			Oxygen01 = Pawn->GetOxygenPercent();
		}
	}
	return Oxygen01 <= LowOxygenThreshold01;
}

// ---------------------------------------------------------------------------
// IsExposed
// ---------------------------------------------------------------------------

UMistspireBTCondition_IsExposed::UMistspireBTCondition_IsExposed()
{
	NodeName = TEXT("Mistspire: Is Exposed");
}

bool UMistspireBTCondition_IsExposed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	return Blackboard && ExposedKey.IsSet() &&
		Blackboard->GetValue<UBlackboardKeyType_Bool>(ExposedKey.GetSelectedKeyID());
}

// ---------------------------------------------------------------------------
// IsAtGoal
// ---------------------------------------------------------------------------

UMistspireBTCondition_IsAtGoal::UMistspireBTCondition_IsAtGoal()
{
	NodeName = TEXT("Mistspire: Is At Goal");
}

bool UMistspireBTCondition_IsAtGoal::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* Actor = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Actor)
	{
		Actor = OwnerComp.GetOwner();
	}
	if (!Blackboard || !MoveTargetKey.IsSet() || !Actor)
	{
		return false;
	}
	const FVector Target = Blackboard->GetValue<UBlackboardKeyType_Vector>(MoveTargetKey.GetSelectedKeyID());
	return FVector::DistSquared(Actor->GetActorLocation(), Target) <= FMath::Square(ToleranceCm);
}