#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireAltitudeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMistspireAltitudeRecord, float, NewAltitudeCm, float, PersonalBestCm);

UCLASS()
class MISTSPIRE_API UMistspireAltitudeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Altitude")
	void UpdateAltitudeFromWorldLocation(const FVector& WorldLocation);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Altitude")
	float GetCurrentAltitudeCm() const { return CurrentAltitudeCm; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Altitude")
	float GetPersonalBestAltitudeCm() const { return PersonalBestAltitudeCm; }

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Altitude")
	FOnMistspireAltitudeRecord OnAltitudeRecord;

private:
	float CurrentAltitudeCm = 0.f;
	float PersonalBestAltitudeCm = 0.f;
};
