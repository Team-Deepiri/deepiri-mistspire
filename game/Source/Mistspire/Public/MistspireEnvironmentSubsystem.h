#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Systems/MistspireBiomeSubsystem.h"
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
 * Handles immersive environment factors like wind, mist density, weather, and biome detection.
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

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetDayNightBlend() const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	float GetAuroraIntensity(float AltitudeCm) const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Environment")
	void ForceWeather(EMistspireWeatherType Weather, float HoldSeconds = 60.f);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	FText GetWeatherDisplayName() const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	EMistspireBiomeType GetCurrentBiome() const { return CurrentBiome; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Environment")
	static EMistspireBiomeType BiomeFromAltitude(float AltitudeCm);

private:
	void UpdateWeather(float DeltaTime);
	void UpdateWeatherPresentation(float DeltaTime);
	void ResolveSkydomeActors();
	void UpdateSkydomeCoverage();
	void ApplyWeatherToSkyActors(const FLinearColor& SkyTint, const FLinearColor& Rayleigh, const FLinearColor& FogColor,
		float FogDensity, const FLinearColor& SunColor, float SunIntensityScale);

	struct FWeatherSkyLook
	{
		FLinearColor SkyTint = FLinearColor(1.f, 1.f, 1.f);
		FLinearColor Rayleigh = FLinearColor(0.175287f, 0.409607f, 1.f);
		FLinearColor FogColor = FLinearColor(0.65f, 0.72f, 0.85f);
		float FogDensity = 0.02f;
		FLinearColor SunColor = FLinearColor(1.f, 0.96f, 0.88f);
		float SunIntensityScale = 1.f;
	};

	static FWeatherSkyLook MakeWeatherSkyLook(EMistspireWeatherType Weather);

	float TimeAccumulator = 0.f;
	float WeatherTransitionTimer = 0.f;
	float ForcedWeatherHold = 0.f;

	FWeatherSkyLook AppliedSkyLook;
	bool bSkyLookInitialized = false;
	bool bCachedSunIntensity = false;
	float CachedSunIntensity = 10.f;
	EMistspireWeatherType PresentedWeather = EMistspireWeatherType::Clear;

	TArray<TWeakObjectPtr<AActor>> SkyDomeActors;
	bool bSkydomeActorsResolved = false;
	float SkydomeMinUniformScale = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Environment", meta = (AllowPrivateAccess = "true"))
	EMistspireWeatherType CurrentWeather = EMistspireWeatherType::Clear;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Environment", meta = (AllowPrivateAccess = "true"))
	EMistspireBiomeType CurrentBiome = EMistspireBiomeType::None;
};
