#include "MistspireEnvironmentSubsystem.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireGameState.h"

void UMistspireEnvironmentSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeAccumulator += DeltaTime;
	UpdateWeather(DeltaTime);
}

TStatId UMistspireEnvironmentSubsystem::GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireEnvironmentSubsystem, STATGROUP_Tickables); }

void UMistspireEnvironmentSubsystem::UpdateWeather(float DeltaTime)
{
	if (ForcedWeatherHold > 0.f)
	{
		ForcedWeatherHold -= DeltaTime;
		return;
	}

	if (!GetWorld()->IsNetMode(NM_Client))
	{
		WeatherTransitionTimer -= DeltaTime;
		if (WeatherTransitionTimer <= 0.f)
		{
			WeatherTransitionTimer = FMath::FRandRange(30.f, 120.f);
			
			float Roll = FMath::FRand();
			if (Roll < 0.6f) CurrentWeather = EMistspireWeatherType::Clear;
			else if (Roll < 0.8f) CurrentWeather = EMistspireWeatherType::MistStorm;
			else if (Roll < 0.95f) CurrentWeather = EMistspireWeatherType::ElectricTurmoil;
			else CurrentWeather = EMistspireWeatherType::ZenithGlow;

			// Sync to GameState
			if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
			{
				GS->CurrentWeatherIndex = (uint8)CurrentWeather;
			}
		}
	}
	else
	{
		// Clients follow the GameState
		if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
		{
			CurrentWeather = (EMistspireWeatherType)GS->CurrentWeatherIndex;
		}
	}
}

FVector UMistspireEnvironmentSubsystem::GetWindAtAltitude(float AltitudeCm) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			if (Interior->IsInsideInterior())
			{
				return FVector::ZeroVector;
			}
		}
	}

	// Wind strength increases with altitude
	float BaseStrength = (AltitudeCm / 100000.f) * 300.f; 
	
	// Weather Multiplier
	float WeatherMult = 1.0f;
	switch (CurrentWeather)
	{
		case EMistspireWeatherType::MistStorm: WeatherMult = 2.5f; break;
		case EMistspireWeatherType::ElectricTurmoil: WeatherMult = 1.8f; break;
		case EMistspireWeatherType::ZenithGlow: WeatherMult = 0.5f; break;
		default: break;
	}

	// Vary direction over time and altitude for turbulence
	float DirTime = GetWorld()->GetTimeSeconds();
	float Angle = (DirTime * 0.2f) + (AltitudeCm * 0.00005f);
	
	FVector WindDir(FMath::Cos(Angle), FMath::Sin(Angle), 0.1f * FMath::Sin(DirTime * 0.5f));
	FVector BaseWind = WindDir * BaseStrength * WeatherMult;

	// Dynamic Gusts (scale with weather)
	float GustFrequency = (0.1f + (AltitudeCm * 0.000001f)) * WeatherMult;
	float GustSeed = DirTime * GustFrequency;
	float GustStrength = FMath::PerlinNoise1D(GustSeed);
	
	if (GustStrength > 0.6f)
	{
		float ExtraPush = (GustStrength - 0.6f) * 1500.f * WeatherMult;
		BaseWind += WindDir * ExtraPush;
	}

	return BaseWind;
}

float UMistspireEnvironmentSubsystem::GetMistDensityAtAltitude(float AltitudeCm) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			if (Interior->IsInsideInterior())
			{
				return 0.08f;
			}
		}
	}

	float Density = FMath::Clamp(1.0f - (AltitudeCm / 150000.f), 0.05f, 1.0f);
	switch (CurrentWeather)
	{
		case EMistspireWeatherType::MistStorm: Density = FMath::Min(1.f, Density * 1.6f); break;
		case EMistspireWeatherType::ZenithGlow: Density *= 0.4f; break;
		default: break;
	}
	return Density;
}

float UMistspireEnvironmentSubsystem::GetDayNightBlend() const
{
	return 0.5f + 0.5f * FMath::Sin(TimeAccumulator * 0.02f);
}

float UMistspireEnvironmentSubsystem::GetAuroraIntensity(float AltitudeCm) const
{
	if (CurrentWeather != EMistspireWeatherType::ZenithGlow && CurrentWeather != EMistspireWeatherType::Clear)
	{
		return 0.f;
	}
	const float AltFactor = FMath::Clamp((AltitudeCm - 400000.f) / 400000.f, 0.f, 1.f);
	return AltFactor * (1.f - GetDayNightBlend()) * 0.85f;
}

void UMistspireEnvironmentSubsystem::ForceWeather(EMistspireWeatherType Weather, float HoldSeconds)
{
	CurrentWeather = Weather;
	ForcedWeatherHold = HoldSeconds;
	WeatherTransitionTimer = HoldSeconds;
	if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
	{
		GS->CurrentWeatherIndex = static_cast<uint8>(Weather);
	}
}

FText UMistspireEnvironmentSubsystem::GetWeatherDisplayName() const
{
	switch (CurrentWeather)
	{
		case EMistspireWeatherType::MistStorm: return NSLOCTEXT("Mistspire", "WeatherMist", "Mist Storm");
		case EMistspireWeatherType::ElectricTurmoil: return NSLOCTEXT("Mistspire", "WeatherElectric", "Electric Turmoil");
		case EMistspireWeatherType::ZenithGlow: return NSLOCTEXT("Mistspire", "WeatherZenith", "Zenith Glow");
		default: return NSLOCTEXT("Mistspire", "WeatherClear", "Clear");
	}
}

float UMistspireEnvironmentSubsystem::GetAtmosphericPressure(float AltitudeCm) const
{
	// Logarithmic pressure drop (Standard Atmosphere approx)
	// Base is 1.0 (Sea Level), dropping to ~0.2 at 12km (Zenith)
	float Pressure = FMath::Exp(-AltitudeCm / 800000.f); 
	return FMath::Clamp(Pressure, 0.05f, 1.0f);
}

float UMistspireEnvironmentSubsystem::GetTemperatureCelsius(float AltitudeCm) const
{
	// Lapse rate: -6.5C per km
	float BaseTemp = 25.0f; // Valley is warm
	float LapseRate = 0.0065f; // per cm
	float Temp = BaseTemp - (AltitudeCm * LapseRate);
	return Temp;
}
