#include "Systems/MistspireSubsystem_02.h"

UMistspireBiomeArid::UMistspireBiomeArid()
{
	MinAltitudeCm = 100000.f;
	MaxAltitudeCm = 300000.f;
	VisibilityRangeCm = 80000.f;
	BiomeAudio.AmbientVolume = 0.4f;
	BiomeAudio.WindPitchBias = 1.3f;
	BiomeAudio.ReverbAmount = 0.6f;
	BiomeAudio.bMuffledSounds = false;
	BiomeAudio.AmbientCue = TEXT("ambient_arid");
	ActiveHazard = EMistspireBiomeHazard::SiroccoStorm;
	HazardCooldown = 120.f;
}

void UMistspireBiomeArid::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = FMath::Lerp(0.15f, 0.05f, AltNormal);
	OutWindStrength = FMath::Lerp(80.f, 200.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(10.f, -2.f, AltNormal);
}

void UMistspireBiomeArid::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 200000.f && Alt < 280000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::SiroccoStorm;
		}
	}
	else
	{
		HazardTimer = FMath::Max(0.f, HazardTimer - DeltaTime);
		if (HazardTimer <= 0.f)
		{
			bHazardActive = false;
		}
	}
}
