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
	return WindDir * BaseStrength;
}

float UMistspireEnvironmentSubsystem::GetMistDensityAtAltitude(float AltitudeCm) const
{
	// Dense mist in the lowlands, clearing as you ascend
	float Density = FMath::Clamp(1.0f - (AltitudeCm / 150000.f), 0.05f, 1.0f);
	return Density;
}
