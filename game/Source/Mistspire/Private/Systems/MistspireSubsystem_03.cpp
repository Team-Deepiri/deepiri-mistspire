#include "Systems/MistspireSubsystem_03.h"

UMistspireBiomeForest::UMistspireBiomeForest()
{
	MinAltitudeCm = 300000.f;
	MaxAltitudeCm = 500000.f;
	VisibilityRangeCm = 20000.f;
	BiomeAudio.AmbientVolume = 0.7f;
	BiomeAudio.WindPitchBias = 0.9f;
	BiomeAudio.ReverbAmount = 0.7f;
	BiomeAudio.bMuffledSounds = true;
	BiomeAudio.AmbientCue = TEXT("ambient_forest");
	ActiveHazard = EMistspireBiomeHazard::ToxicHaze;
	HazardCooldown = 100.f;
}

void UMistspireBiomeForest::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = FMath::Lerp(0.5f, 0.2f, AltNormal);
	OutWindStrength = FMath::Lerp(30.f, 100.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-3.f, -10.f, AltNormal);
}

void UMistspireBiomeForest::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 400000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::ToxicHaze;
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
