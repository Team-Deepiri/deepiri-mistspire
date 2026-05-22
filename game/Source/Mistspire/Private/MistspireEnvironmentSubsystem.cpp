#include "MistspireEnvironmentSubsystem.h"

void UMistspireEnvironmentSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeAccumulator += DeltaTime;
	UpdateWeather(DeltaTime);
}

TStatId UMistspireEnvironmentSubsystem::GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireEnvironmentSubsystem, STATGROUP_Tickables); }

void UMistspireEnvironmentSubsystem::UpdateWeather(float DeltaTime)
{
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
	// Dense mist in the lowlands, clearing as you ascend
	float Density = FMath::Clamp(1.0f - (AltitudeCm / 150000.f), 0.05f, 1.0f);
	return Density;
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
