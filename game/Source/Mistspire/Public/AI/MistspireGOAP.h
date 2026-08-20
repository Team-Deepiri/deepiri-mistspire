#pragma once

#include "CoreMinimal.h"
#include "MistspireGOAP.generated.h"

class UMistspireGOAPPlanner;

/** A named boolean fact of the world (e.g. HasOxygen, IsSheltered). */
USTRUCT(BlueprintType)
struct FMistspireGOAPFact
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Key = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bValue = true;
};

/** Searchable world state: a set of boolean facts plus a numeric value map. */
USTRUCT(BlueprintType)
struct FMistspireGOAPState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) TMap<FName, bool> Facts;
	UPROPERTY(BlueprintReadWrite) TMap<FName, float> Values;

	bool operator==(const FMistspireGOAPState& Other) const;
	bool operator!=(const FMistspireGOAPState& Other) const { return !(*this == Other); }

	/** True when every fact/value in Goal matches this state. */
	bool Satisfies(const FMistspireGOAPState& Goal) const;
	void Apply(const FMistspireGOAPState& Delta);

	/** Canonical string used as a visited-set key during search. */
	FString ToKeyString() const;
};

/** An atomic planner action with preconditions, effects and a cost. */
USTRUCT(BlueprintType)
struct FMistspireGOAPAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ActionName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Cost = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FMistspireGOAPFact> Preconditions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FMistspireGOAPFact> Effects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, float> ValuePreconditions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName, float> ValueEffects;

	/** The preconditions/effects expressed as a state delta. */
	FMistspireGOAPState GetPreconditions() const;
	FMistspireGOAPState GetEffects() const;
	bool IsValid() const { return ActionName != NAME_None; }
};

/** Goal-oriented action planning: A* over facts/values to find the cheapest action plan. */
UCLASS(BlueprintType)
class MISTSPIRE_API UMistspireGOAPPlanner : public UObject
{
	GENERATED_BODY()

public:
	/** Finds the cheapest plan from Start that satisfies Goal, using the given action set. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|GOAP")
	static bool Plan(
		const FMistspireGOAPState& Start,
		const FMistspireGOAPState& Goal,
		const TArray<FMistspireGOAPAction>& Actions,
		TArray<FMistspireGOAPAction>& OutPlan,
		int32 MaxDepth = 16);

	/** Default Mistspire action library: climb, shelter, oxygen, beacon, rest. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|GOAP")
	static TArray<FMistspireGOAPAction> BuildMistspireActionLibrary();

	/** Convenience: build a goal state from a list of facts and value thresholds. */
	UFUNCTION(BlueprintPure, Category = "Mistspire|AI|GOAP")
	static FMistspireGOAPState MakeGoal(const TArray<FMistspireGOAPFact>& Facts, const TMap<FName, float>& Values);
};