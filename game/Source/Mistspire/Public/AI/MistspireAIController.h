#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MistspireAITypes.h"
#include "MistspireGOAP.h"
#include "MistspireUtilityAI.h"
#include "MistspireSteering.h"
#include "MistspireAIController.generated.h"

class UMistspireStateMachineComponent;

/** Game AI controller: Behavior Tree ready, with built-in Utility AI + GOAP fallback. */
UCLASS()
class MISTSPIRE_API AMistspireAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMistspireAIController();

	virtual void Tick(float DeltaTime) override;

	/** Steering behavior used when moving freely through the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|AI")
	TObjectPtr<UMistspireSteeringComponent> Steering;

	/** Generic state chart attached to this controller's pawn logic. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|AI")
	TObjectPtr<UMistspireStateMachineComponent> StateMachine;

	/** The GOAP action library this agent can plan with. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|GOAP")
	TArray<FMistspireGOAPAction> ActionLibrary;

	/** Weights for the default climber utility options (Rest / ClimbOn / FindShelter), applied when the evaluator is built. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Utility")
	FMistspireUtilityClimberTuning ClimberTuning;

	/** Pushes a fresh snapshot of survival/position inputs into the evaluator. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI")
	void UpdateWorldState(const FMistspireAIWorldState& NewState);

	/** Runs the Utility AI and returns the winning decision. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Utility")
	FMistspireUtilityDecision RunUtilityDecision();

	/** Plans toward a goal with GOAP. Returns false when no plan exists. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|GOAP")
	bool PlanGOAP(const FMistspireGOAPState& Goal, TArray<FMistspireGOAPAction>& OutPlan, int32 MaxDepth = 12);

	/** Builds a GOAP start state from the latest world-state snapshot. */
	UFUNCTION(BlueprintPure, Category = "Mistspire|AI|GOAP")
	static FMistspireGOAPState BuildGOAPStartState(const FMistspireAIWorldState& WorldState);

	/** Samples survival/position facts from the local player pawn. */
	UFUNCTION(BlueprintPure, Category = "Mistspire|AI")
	static FMistspireAIWorldState SnapshotFromPawn(const APawn* SourcePawn, const UWorld* World);

	/** Steer the pawn to a location. Uses the pawn's movement component when present; free-flight pawns without one are moved directly (no physics sweeps). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Movement")
	void MoveSteeredTo(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Movement")
	void WanderAround(const FVector& Center);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Movement")
	void StopSteering();

	UFUNCTION(BlueprintPure, Category = "Mistspire|AI")
	bool IsCurrentlySteering() const { return bSteeringActive; }

private:
	UPROPERTY(Transient)
	TObjectPtr<UMistspireUtilityEvaluator> UtilityEvaluator;

	bool bSteeringActive = false;
	FMistspireAIWorldState WorldState;
};