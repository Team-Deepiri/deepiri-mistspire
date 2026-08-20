#include "MistspireUtilityAI.h"
#include "Curves/CurveFloat.h"

float UMistspireUtilityConsideration::Score(float InputValue) const
{
	const float Clamped = FMath::Clamp(InputValue, 0.f, 1.f);

	float Result = 0.f;
	if (ResponseCurve)
	{
		Result = ResponseCurve->GetFloatValue(Clamped);
	}
	else
	{
		Result = Clamped;
	}

	if (bInverted)
	{
		Result = 1.f - Result;
	}
	return FMath::Clamp(Result, 0.f, 1.f);
}

void UMistspireUtilityEvaluator::SetInput(FName InputName, float Value01)
{
	Inputs.FindOrAdd(InputName) = FMath::Clamp(Value01, 0.f, 1.f);
}

float UMistspireUtilityEvaluator::ScoreOption(FName OptionName) const
{
	for (const FMistspireUtilityOption& Option : Options)
	{
		if (Option.Name != OptionName)
		{
			continue;
		}
		if (Option.Considerations.Num() == 0)
		{
			return 0.f;
		}
		float Product = 1.f;
		for (const TObjectPtr<UMistspireUtilityConsideration>& Consideration : Option.Considerations)
		{
			if (!Consideration)
			{
				continue;
			}
			const float* Input = Inputs.Find(Consideration->InputName);
			Product *= Consideration->Score(Input ? *Input : 0.f);
		}
		return Product * FMath::Max(Option.Weight, 0.f);
	}
	return 0.f;
}

FMistspireUtilityDecision UMistspireUtilityEvaluator::EvaluateBest() const
{
	FMistspireUtilityDecision Best;
	for (const FMistspireUtilityOption& Option : Options)
	{
		const float Score = ScoreOption(Option.Name);
		if (Score > Best.Score)
		{
			Best.Score = Score;
			Best.DecisionName = Option.Name;
			Best.bValid = true;
		}
	}
	return Best;
}

UMistspireUtilityEvaluator* UMistspireUtilityEvaluator::MakeClimberEvaluator(UObject* Outer)
{
	UMistspireUtilityEvaluator* Evaluator = NewObject<UMistspireUtilityEvaluator>(Outer);
	Evaluator->Options.SetNum(3);

	// Rest — high score when stamina AND oxygen are low.
	{
		FMistspireUtilityOption& Rest = Evaluator->Options[0];
		Rest.Name = TEXT("Rest");
		Rest.Weight = 1.f;
		Rest.Considerations.SetNum(2);
		Rest.Considerations[0] = NewObject<UMistspireUtilityConsideration>(Evaluator);
		Rest.Considerations[0]->InputName = TEXT("Stamina");
		Rest.Considerations[0]->bInverted = true;
		Rest.Considerations[1] = NewObject<UMistspireUtilityConsideration>(Evaluator);
		Rest.Considerations[1]->InputName = TEXT("Oxygen");
		Rest.Considerations[1]->bInverted = true;
	}

	// ClimbOn — high score when resources are healthy.
	{
		FMistspireUtilityOption& Climb = Evaluator->Options[1];
		Climb.Name = TEXT("ClimbOn");
		Climb.Weight = 1.2f;
		Climb.Considerations.SetNum(2);
		Climb.Considerations[0] = NewObject<UMistspireUtilityConsideration>(Evaluator);
		Climb.Considerations[0]->InputName = TEXT("Stamina");
		Climb.Considerations[1] = NewObject<UMistspireUtilityConsideration>(Evaluator);
		Climb.Considerations[1]->InputName = TEXT("Oxygen");
	}

	// FindShelter — high score when exposed and warm-when-exposed is dangerous.
	{
		FMistspireUtilityOption& Shelter = Evaluator->Options[2];
		Shelter.Name = TEXT("FindShelter");
		Shelter.Weight = 1.5f;
		Shelter.Considerations.SetNum(2);
		Shelter.Considerations[0] = NewObject<UMistspireUtilityConsideration>(Evaluator);
		Shelter.Considerations[0]->InputName = TEXT("Exposure");
		Shelter.Considerations[1] = NewObject<UMistspireUtilityConsideration>(Evaluator);
		Shelter.Considerations[1]->InputName = TEXT("Stamina");
		Shelter.Considerations[1]->bInverted = true;
	}

	return Evaluator;
}