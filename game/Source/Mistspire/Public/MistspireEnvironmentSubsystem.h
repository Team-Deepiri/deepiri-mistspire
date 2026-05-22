#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireEnvironmentSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMistspireWeatherType : uint8
{
	Clear,
	MistStorm,
	ElectricTurmoil,
	ZenithGlow
};

/**
 * Handles immersive environment factors like wind, mist density, and weather.
 */
UCLASS()
class MISTSPIRE_API UMistspireEnvironmentSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	FVector GetWindAtAltitude(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetMistDensityAtAltitude(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetAtmosphericPressure(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetTemperatureCelsius(float AltitudeCm) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	EMistspireWeatherType GetCurrentWeather() const { return CurrentWeather; }

private:
	void UpdateWeather(float DeltaTime);

	float TimeAccumulator = 0.f;
	float WeatherTransitionTimer = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Environment", meta = (AllowPrivateAccess = "true"))
	EMistspireWeatherType CurrentWeather = EMistspireWeatherType::Clear;
};
