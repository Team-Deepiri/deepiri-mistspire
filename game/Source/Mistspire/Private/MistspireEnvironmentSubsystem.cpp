#include "MistspireEnvironmentSubsystem.h"

void UMistspireEnvironmentSubsystem::Tick(float DeltaTime)
{
	TimeAccumulator += DeltaTime;
}

FVector UMistspireEnvironmentSubsystem::GetWindAtAltitude(float AltitudeCm) const
{
	// Wind strength increases with altitude
	float BaseStrength = (AltitudeCm / 100000.f) * 300.f; 
	
	// Vary direction over time and altitude for turbulence
	float DirTime = GetWorld()->GetTimeSeconds();
	float Angle = (DirTime * 0.2f) + (AltitudeCm * 0.00005f);
	
	FVector WindDir(FMath::Cos(Angle), FMath::Sin(Angle), 0.1f * FMath::Sin(DirTime * 0.5f));
	FVector BaseWind = WindDir * BaseStrength;

	// Dynamic Gusts
	float GustFrequency = 0.1f + (AltitudeCm * 0.000001f); // More frequent gusts at height
	float GustSeed = DirTime * GustFrequency;
	float GustStrength = FMath::PerlinNoise1D(GustSeed);
	
	if (GustStrength > 0.6f)
	{
		float ExtraPush = (GustStrength - 0.6f) * 1500.f;
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
