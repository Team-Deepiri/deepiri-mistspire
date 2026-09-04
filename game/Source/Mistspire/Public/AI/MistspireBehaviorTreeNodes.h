#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "MistspireBehaviorTreeNodes.generated.h"

/**
 * Behavior Tree nodes for Mistspire AI.
 * Assemble them into a Behavior Tree asset in the editor; they pair with the
 * blackboard keys listed on each node.
 */

/** Climbs the owning actor vertically toward the `ClimbGoalCm` blackboard float. */
UCLASS()
class MISTSPIRE_API UMistspireBTTask_ClimbToAltitude : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMistspireBTTask_ClimbToAltitude();

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	FBlackboardKeySelector ClimbGoalKey;

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	float ClimbSpeedCmPerSec = 500.f;

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	float ToleranceCm = 40.f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

/** Mistspire: Move To Location task instance memory. */
struct FBTMoveToLocationMemory
{
	FVector TargetLocation = FVector::ZeroVector;
	FAIRequestID MoveRequestId;
	bool bMoveRequested = false;
	bool bWasMoving = false;
};

/** Moves the AI pawn to the `MoveTarget` blackboard vector via navmesh movement. */
UCLASS()
class MISTSPIRE_API UMistspireBTTask_MoveToLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMistspireBTTask_MoveToLocation();

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	FBlackboardKeySelector MoveTargetKey;

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	float AcceptanceRadiusCm = 150.f;

	virtual uint16 GetInstanceMemorySize() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

/** Simulates refilling oxygen (shelter/canister contact). Immediate success. */
UCLASS()
class MISTSPIRE_API UMistspireBTTask_RefillOxygen : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

/** Plays a dialogue line by id (`DialogueLine` blackboard name) through the dialogue subsystem. */
UCLASS()
class MISTSPIRE_API UMistspireBTTask_PlayDialogueLine : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMistspireBTTask_PlayDialogueLine();

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	FBlackboardKeySelector DialogueLineKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

/** True when the AI pawn's oxygen is below a threshold. */
UCLASS()
class MISTSPIRE_API UMistspireBTCondition_IsLowOxygen : public UBTDecorator
{
	GENERATED_BODY()

public:
	UMistspireBTCondition_IsLowOxygen();

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	FBlackboardKeySelector OxygenPercentKey;

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	float LowOxygenThreshold01 = 0.25f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

/** True when the `bExposed` blackboard bool is set (dangerous sky). */
UCLASS()
class MISTSPIRE_API UMistspireBTCondition_IsExposed : public UBTDecorator
{
	GENERATED_BODY()

public:
	UMistspireBTCondition_IsExposed();

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	FBlackboardKeySelector ExposedKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};

/** True when the AI pawn is within tolerance of the `MoveTarget` blackboard vector. */
UCLASS()
class MISTSPIRE_API UMistspireBTCondition_IsAtGoal : public UBTDecorator
{
	GENERATED_BODY()

public:
	UMistspireBTCondition_IsAtGoal();

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	FBlackboardKeySelector MoveTargetKey;

	UPROPERTY(EditAnywhere, Category = "Mistspire")
	float ToleranceCm = 200.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};