#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireEnvironmentSubsystem.generated.h"

/**
 * Handles immersive environment factors like wind and mist density based on altitude.
 */
UCLASS()
class MISTSPIRE_API UMistspireEnvironmentSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	FVector GetWindAtAltitude(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetMistDensityAtAltitude(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetAtmosphericPressure(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetTemperatureCelsius(float AltitudeCm) const;

private:
	float TimeAccumulator = 0.f;
};
