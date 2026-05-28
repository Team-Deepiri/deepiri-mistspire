#include "Systems/MistspireSubsystem_04.h"

UMistspireBiomeEmber::UMistspireBiomeEmber()
{
	MinAltitudeCm = 500000.f;
	MaxAltitudeCm = 700000.f;
	VisibilityRangeCm = 50000.f;
	BiomeAudio.AmbientVolume = 0.5f;
	BiomeAudio.WindPitchBias = 0.6f;
	BiomeAudio.ReverbAmount = 0.8f;
	BiomeAudio.bMuffledSounds = false;
	BiomeAudio.AmbientCue = TEXT("ambient_ember");
	ActiveHazard = EMistspireBiomeHazard::MagmaticBurst;
	HazardCooldown = 80.f;
}

void UMistspireBiomeEmber::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = FMath::Lerp(0.3f, 0.1f, AltNormal);
	OutWindStrength = FMath::Lerp(50.f, 150.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(15.f, 5.f, AltNormal);
}

void UMistspireBiomeEmber::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 550000.f && Alt < 680000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::MagmaticBurst;
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
