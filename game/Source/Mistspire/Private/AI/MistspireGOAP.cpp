#include "AI/MistspireGOAP.h"
#include "Misc/Char.h"

bool FMistspireGOAPState::operator==(const FMistspireGOAPState& Other) const
{
	if (Facts.Num() != Other.Facts.Num() || Values.Num() != Other.Values.Num())
	{
		return false;
	}
	for (const TPair<FName, bool>& Pair : Facts)
	{
		const bool* OtherValue = Other.Facts.Find(Pair.Key);
		if (!OtherValue || *OtherValue != Pair.Value)
		{
			return false;
		}
	}
	for (const TPair<FName, float>& Pair : Values)
	{
		const float* OtherValue = Other.Values.Find(Pair.Key);
		if (!OtherValue || !FMath::IsNearlyEqual(*OtherValue, Pair.Value, 0.01f))
		{
			return false;
		}
	}
	return true;
}

bool FMistspireGOAPState::Satisfies(const FMistspireGOAPState& Goal) const
{
	for (const TPair<FName, bool>& Pair : Goal.Facts)
	{
		const bool* Have = Facts.Find(Pair.Key);
		if (!Have || *Have != Pair.Value)
		{
			return false;
		}
	}
	for (const TPair<FName, float>& Pair : Goal.Values)
	{
		const float* Have = Values.Find(Pair.Key);
		if (!Have || *Have < Pair.Value)
		{
			return false;
		}
	}
	return true;
}

void FMistspireGOAPState::Apply(const FMistspireGOAPState& Delta)
{
	for (const TPair<FName, bool>& Pair : Delta.Facts)
	{
		Facts.FindOrAdd(Pair.Key) = Pair.Value;
	}
	for (const TPair<FName, float>& Pair : Delta.Values)
	{
		Values.FindOrAdd(Pair.Key) = Pair.Value;
	}
}

FString FMistspireGOAPState::ToKeyString() const
{
	TArray<FName> Keys;
	Facts.GetKeys(Keys);
	Keys.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	FString Out;
	for (const FName& Key : Keys)
	{
		Out += FString::Printf(TEXT("%s:%d;"), *Key.ToString(), Facts[Key] ? 1 : 0);
	}

	TArray<FName> ValueKeys;
	Values.GetKeys(ValueKeys);
	ValueKeys.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });
	for (const FName& Key : ValueKeys)
	{
		Out += FString::Printf(TEXT("%s=%.1f;"), *Key.ToString(), Values[Key]);
	}
	return Out;
}

FMistspireGOAPState FMistspireGOAPAction::GetPreconditions() const
{
	FMistspireGOAPState State;
	for (const FMistspireGOAPFact& Fact : Preconditions)
	{
		if (Fact.Key != NAME_None)
		{
			State.Facts.FindOrAdd(Fact.Key) = Fact.bValue;
		}
	}
	for (const TPair<FName, float>& Pair : ValuePreconditions)
	{
		State.Values.FindOrAdd(Pair.Key) = Pair.Value;
	}
	return State;
}

FMistspireGOAPState FMistspireGOAPAction::GetEffects() const
{
	FMistspireGOAPState State;
	for (const FMistspireGOAPFact& Fact : Effects)
	{
		if (Fact.Key != NAME_None)
		{
			State.Facts.FindOrAdd(Fact.Key) = Fact.bValue;
		}
	}
	for (const TPair<FName, float>& Pair : ValueEffects)
	{
		State.Values.FindOrAdd(Pair.Key) = Pair.Value;
	}
	return State;
}

FMistspireGOAPState UMistspireGOAPPlanner::MakeGoal(const TArray<FMistspireGOAPFact>& Facts, const TMap<FName, float>& Values)
{
	FMistspireGOAPState Goal;
	for (const FMistspireGOAPFact& Fact : Facts)
	{
		if (Fact.Key != NAME_None)
		{
			Goal.Facts.FindOrAdd(Fact.Key) = Fact.bValue;
		}
	}
	Goal.Values = Values;
	return Goal;
}

TArray<FMistspireGOAPAction> UMistspireGOAPPlanner::BuildMistspireActionLibrary()
{
	// Prototype tuning constants for the default climber action set.
	TArray<FMistspireGOAPAction> Actions;
	Actions.SetNum(6);

	// 1. ClimbHigher — requires stamina + oxygen, costs stamina, raises altitude.
	{
		FMistspireGOAPAction& Climb = Actions[0];
		Climb.ActionName = TEXT("ClimbHigher");
		Climb.Cost = 2.f;
		Climb.Preconditions = { { TEXT("HasStamina"), true }, { TEXT("HasOxygen"), true } };
		Climb.Effects = { { TEXT("Climbing"), true } };
		Climb.ValueEffects.Add(TEXT("Altitude"), 100.f);
	}

	// 2. SeekShelter — removes exposure, restores warmth.
	{
		FMistspireGOAPAction& Shelter = Actions[1];
		Shelter.ActionName = TEXT("SeekShelter");
		Shelter.Cost = 3.f;
		Shelter.Preconditions = { { TEXT("ShelterKnown"), true } };
		Shelter.Effects = { { TEXT("IsSheltered"), true }, { TEXT("Exposed"), false } };
		Shelter.ValueEffects.Add(TEXT("Warmth"), 100.f);
	}

	// 3. RefillOxygen — requires a canister or shelter, restores oxygen.
	{
		FMistspireGOAPAction& Oxygen = Actions[2];
		Oxygen.ActionName = TEXT("RefillOxygen");
		Oxygen.Cost = 1.5f;
		Oxygen.Preconditions = { { TEXT("CanisterNearby"), true } };
		Oxygen.Effects = { { TEXT("HasOxygen"), true } };
		Oxygen.ValueEffects.Add(TEXT("Oxygen"), 100.f);
	}

	// 4. ReachBeacon — the primary long-term goal, highest cost, only useful at end.
	{
		FMistspireGOAPAction& Beacon = Actions[3];
		Beacon.ActionName = TEXT("ReachBeacon");
		Beacon.Cost = 8.f;
		Beacon.Preconditions = { { TEXT("Climbing"), true }, { TEXT("HasOxygen"), true } };
		Beacon.Effects = { { TEXT("BeaconReached"), true } };
	}

	// 5. RestoreStamina — low cost recovery when exhausted.
	{
		FMistspireGOAPAction& Rest = Actions[4];
		Rest.ActionName = TEXT("RestoreStamina");
		Rest.Cost = 1.f;
		Rest.Effects = { { TEXT("HasStamina"), true }, { TEXT("Climbing"), false } };
		Rest.ValueEffects.Add(TEXT("Stamina"), 100.f);
	}

	// 6. UseWindCrystal — emergency stamina restore that does not stop climbing.
	{
		FMistspireGOAPAction& Crystal = Actions[5];
		Crystal.ActionName = TEXT("UseWindCrystal");
		Crystal.Cost = 0.8f;
		Crystal.Preconditions = { { TEXT("HasCrystal"), true } };
		Crystal.Effects = { { TEXT("HasStamina"), true } };
		Crystal.ValueEffects.Add(TEXT("Stamina"), 60.f);
	}

	return Actions;
}

bool UMistspireGOAPPlanner::Plan(
	const FMistspireGOAPState& Start,
	const FMistspireGOAPState& Goal,
	const TArray<FMistspireGOAPAction>& Actions,
	TArray<FMistspireGOAPAction>& OutPlan,
	int32 MaxDepth)
{
	OutPlan.Reset();

	// Best-first search with a visited set keyed by canonical state string.
	struct FNode
	{
		FMistspireGOAPState State;
		TArray<FMistspireGOAPAction> Path;
		float Cost = 0.f;
	};

	TArray<FNode> Open;
	TSet<FString> Visited;

	FNode StartNode;
	StartNode.State = Start;
	Open.Add(StartNode);
	Visited.Add(Start.ToKeyString());

	while (Open.Num() > 0)
	{
		// Cheap priority pop: linear scan (action sets are tiny in this game).
		int32 BestIndex = 0;
		for (int32 Idx = 1; Idx < Open.Num(); ++Idx)
		{
			if (Open[Idx].Cost < Open[BestIndex].Cost)
			{
				BestIndex = Idx;
			}
		}
		FNode Current = Open[BestIndex];
		Open.RemoveAtSwap(BestIndex);

		if (Current.Path.Num() >= MaxDepth)
		{
			continue;
		}

		if (Current.State.Satisfies(Goal))
		{
			OutPlan = Current.Path;
			return true;
		}

		for (const FMistspireGOAPAction& Action : Actions)
		{
			const FMistspireGOAPState Preconditions = Action.GetPreconditions();
			if (!Current.State.Satisfies(Preconditions))
			{
				continue;
			}

			FNode Next;
			Next.State = Current.State;
			Next.State.Apply(Action.GetEffects());
			const FString Key = Next.State.ToKeyString();
			if (Visited.Contains(Key))
			{
				continue;
			}

			Next.Path = Current.Path;
			Next.Path.Add(Action);
			Next.Cost = Current.Cost + Action.Cost;
			Visited.Add(Key);
			Open.Add(Next);
		}
	}

	return false;
}