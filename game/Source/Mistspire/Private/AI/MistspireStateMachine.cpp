#include "MistspireStateMachine.h"

UMistspireStateMachineComponent::UMistspireStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMistspireStateMachineComponent::SetStates(const TArray<FMistspireStateDefinition>& InStates, FName InInitialState)
{
	States = InStates;
	InitialState = InInitialState;
	CurrentState = InitialState;
}

bool UMistspireStateMachineComponent::StateAcceptsInput(FName StateId, FName Input) const
{
	for (const FMistspireStateDefinition& State : States)
	{
		if (State.StateId == StateId)
		{
			return State.ValidInputs.Contains(Input);
		}
	}
	return false;
}

bool UMistspireStateMachineComponent::FindTransition(FName Input, FName& OutToState) const
{
	for (const FMistspireStateTransition& Transition : Transitions)
	{
		if (Transition.FromState == CurrentState && Transition.Input == Input)
		{
			OutToState = Transition.ToState;
			return true;
		}
	}
	return false;
}

bool UMistspireStateMachineComponent::PushInput(FName Input)
{
	if (CurrentState == NAME_None)
	{
		CurrentState = InitialState;
		return true;
	}

	FName NextState = NAME_None;
	bool bCanMove = false;

	if (Transitions.Num() > 0)
	{
		// Explicit transition table wins.
		bCanMove = FindTransition(Input, NextState);
	}
	else if (StateAcceptsInput(CurrentState, Input))
	{
		// Free-form chart: any accepting state may take it — pick the first matching.
		for (const FMistspireStateDefinition& State : States)
		{
			if (State.StateId != CurrentState && State.ValidInputs.Contains(Input))
			{
				NextState = State.StateId;
				bCanMove = true;
				break;
			}
		}
	}

	if (bCanMove && NextState != CurrentState)
	{
		const FName OldState = CurrentState;
		CurrentState = NextState;
		OnStateChanged.Broadcast(OldState, CurrentState);
		return true;
	}
	return false;
}