#include "Systems/MistspireSubsystem_05.h"

UMistspireBiomeCrystal::UMistspireBiomeCrystal()
{
	MinAltitudeCm = 700000.f;
	MaxAltitudeCm = 900000.f;
	VisibilityRangeCm = 100000.f;
	BiomeAudio.AmbientVolume = 0.45f;
	BiomeAudio.WindPitchBias = 1.5f;
	BiomeAudio.ReverbAmount = 0.9f;
	BiomeAudio.bMuffledSounds = false;
	BiomeAudio.AmbientCue = TEXT("ambient_crystal");
	ActiveHazard = EMistspireBiomeHazard::ResonantFracture;
	HazardCooldown = 70.f;
}

void UMistspireBiomeCrystal::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = FMath::Lerp(0.1f, 0.02f, AltNormal);
	OutWindStrength = FMath::Lerp(100.f, 250.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-10.f, -25.f, AltNormal);
}

void UMistspireBiomeCrystal::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 780000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::ResonantFracture;
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
