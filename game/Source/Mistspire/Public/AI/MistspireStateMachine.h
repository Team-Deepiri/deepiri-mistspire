#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MistspireAITypes.h"
#include "MistspireStateMachine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMistspireStateChanged, FName, OldState, FName, NewState);

/** Generic state chart for gameplay actors (State Charts / LimboAI equivalent). */
UCLASS(ClassGroup = (Mistspire), meta = (BlueprintSpawnableComponent))
class MISTSPIRE_API UMistspireStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMistspireStateMachineComponent();

	/** All possible states and the inputs each state accepts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|StateMachine")
	TArray<FMistspireStateDefinition> States;

	/** Explicit directed transitions; when empty, any valid input moves anywhere. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|StateMachine")
	TArray<FMistspireStateTransition> Transitions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|StateMachine")
	FName InitialState = NAME_None;

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|AI|StateMachine")
	FOnMistspireStateChanged OnStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|StateMachine")
	void SetStates(const TArray<FMistspireStateDefinition>& InStates, FName InInitialState);

	/** Feeds an input; transitions if the current state accepts it (or a transition exists). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|StateMachine")
	bool PushInput(FName Input);

	UFUNCTION(BlueprintPure, Category = "Mistspire|AI|StateMachine")
	FName GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|AI|StateMachine")
	bool IsInState(FName StateId) const { return CurrentState == StateId; }

private:
	bool FindTransition(FName Input, FName& OutToState) const;
	bool StateAcceptsInput(FName StateId, FName Input) const;

	UPROPERTY(Transient)
	FName CurrentState = NAME_None;
};