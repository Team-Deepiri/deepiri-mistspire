#pragma once

#include "CoreMinimal.h"
#include "MistspireAITypes.h"
#include "MistspireUtilityAI.generated.h"

class UCurveFloat;

/** One scored input to a utility decision; maps a normalized input through a curve. */
UCLASS(BlueprintType)
class MISTSPIRE_API UMistspireUtilityConsideration : public UObject
{
	GENERATED_BODY()

public:
	/** Name of the live input this consideration samples (see UMistspireUtilityEvaluator::SetInput). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Utility")
	FName InputName = NAME_None;

	/** Optional response curve over normalized 0..1 input. Null = identity (linear). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Utility")
	TObjectPtr<UCurveFloat> ResponseCurve = nullptr;

	/** Invert the output so that LOW input scores HIGH. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Utility")
	bool bInverted = false;

	/** Scores the input value through the curve, clamped to 0..1. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Utility")
	float Score(float InputValue) const;
};

/** One candidate decision: a weighted product of considerations. */
USTRUCT(BlueprintType)
struct FMistspireUtilityOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Name = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Weight = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TObjectPtr<UMistspireUtilityConsideration>> Considerations;
};

/** Score-based decision maker: set normalized inputs, evaluate options, pick the best. */
UCLASS(BlueprintType)
class MISTSPIRE_API UMistspireUtilityEvaluator : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Utility")
	TArray<FMistspireUtilityOption> Options;

	/** Feeds a normalized 0..1 input consumed by matching considerations. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Utility")
	void SetInput(FName InputName, float Value01);

	/** Scores every option and returns the highest. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Utility")
	FMistspireUtilityDecision EvaluateBest() const;

	/** Scores a single option by name (0 if missing). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Utility")
	float ScoreOption(FName OptionName) const;

	/** Builds the default Mistspire climber evaluator (Rest / ClimbOn / FindShelter). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Utility")
	static UMistspireUtilityEvaluator* MakeClimberEvaluator(UObject* Outer);

private:
	TMap<FName, float> Inputs;
};