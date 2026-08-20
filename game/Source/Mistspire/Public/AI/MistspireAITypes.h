#pragma once

#include "CoreMinimal.h"
#include "MistspireAITypes.generated.h"

/** Behavioural archetypes an AI actor can belong to. */
UENUM(BlueprintType)
enum class EMistspireAIDemeanor : uint8
{
	Wander,
	Guide,
	HelpfulClimber,
	Avoidant,
	Aggressive,
};

/** Immutable, cheap world-state snapshot used by GOAP and Utility AI. */
USTRUCT(BlueprintType)
struct FMistspireAIWorldState
{
	GENERATED_BODY()

	/** Continuous survival inputs 0..1 (oxygen, stamina, warmth). */
	UPROPERTY(BlueprintReadWrite) float Oxygen01 = 1.f;
	UPROPERTY(BlueprintReadWrite) float Stamina01 = 1.f;
	UPROPERTY(BlueprintReadWrite) float Warmth01 = 1.f;
	/** Current altitude in cm and distance to the next summit. */
	UPROPERTY(BlueprintReadWrite) float AltitudeCm = 0.f;
	UPROPERTY(BlueprintReadWrite) float BeaconDistanceCm = 0.f;
	/** True when the sky is dangerous (electric storm, zenith glare). */
	UPROPERTY(BlueprintReadWrite) bool bExposed = false;
	UPROPERTY(BlueprintReadWrite) bool bHasOxygen = true;
	UPROPERTY(BlueprintReadWrite) bool bHasStamina = true;
	UPROPERTY(BlueprintReadWrite) bool bWantsRest = false;
};

/** Result of a Utility AI evaluation. */
USTRUCT(BlueprintType)
struct FMistspireUtilityDecision
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName DecisionName = NAME_None;
	UPROPERTY(BlueprintReadOnly) float Score = 0.f;
	UPROPERTY(BlueprintReadOnly) bool bValid = false;
};

/** One state in a state chart. */
USTRUCT(BlueprintType)
struct FMistspireStateDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName StateId = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> ValidInputs;
};

/** A directed transition between two states on a given input. */
USTRUCT(BlueprintType)
struct FMistspireStateTransition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName FromState = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ToState = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Input = NAME_None;
};