#include "Systems/MistspireSubsystem_10.h"

UMistspireBiomePinnacle::UMistspireBiomePinnacle()
{
	MinAltitudeCm = 1800000.f;
	MaxAltitudeCm = 2000000.f;
	VisibilityRangeCm = 500000.f;
	BiomeAudio.AmbientVolume = 0.05f;
	BiomeAudio.WindPitchBias = 0.0f;
	BiomeAudio.ReverbAmount = 0.0f;
	BiomeAudio.bMuffledSounds = false;
	BiomeAudio.AmbientCue = TEXT("ambient_pinnacle");
	ActiveHazard = EMistspireBiomeHazard::CosmicRadiation;
	HazardCooldown = 20.f;
}

void UMistspireBiomePinnacle::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = 0.0f;
	OutWindStrength = FMath::Lerp(0.f, 10.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-70.f, -80.f, AltNormal);
}

void UMistspireBiomePinnacle::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 1850000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= FMath::Max(HazardCooldown * FMath::FRandRange(0.5f, 1.5f), 5.f))
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::CosmicRadiation;
			HazardTimer = 0.f;
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
