#include "Systems/MistspireSubsystem_01.h"

UMistspireBiomeMist::UMistspireBiomeMist()
{
	MinAltitudeCm = 0.f;
	MaxAltitudeCm = 100000.f;
	VisibilityRangeCm = 30000.f;
	BiomeAudio.AmbientVolume = 0.6f;
	BiomeAudio.WindPitchBias = 0.8f;
	BiomeAudio.ReverbAmount = 0.4f;
	BiomeAudio.bMuffledSounds = true;
	BiomeAudio.AmbientCue = TEXT("ambient_mist");
	ActiveHazard = EMistspireBiomeHazard::Hypoxia;
	HazardCooldown = 90.f;
}

void UMistspireBiomeMist::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = FMath::Lerp(0.7f, 0.3f, AltNormal);
	OutWindStrength = FMath::Lerp(10.f, 60.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(2.f, -5.f, AltNormal);
}

void UMistspireBiomeMist::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 70000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown * 0.5f)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::Hypoxia;
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
