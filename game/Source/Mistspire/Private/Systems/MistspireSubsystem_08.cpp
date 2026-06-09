#include "Systems/MistspireSubsystem_08.h"

UMistspireBiomeAether::UMistspireBiomeAether()
{
	MinAltitudeCm = 1400000.f;
	MaxAltitudeCm = 1600000.f;
	VisibilityRangeCm = 200000.f;
	BiomeAudio.AmbientVolume = 0.15f;
	BiomeAudio.WindPitchBias = 0.5f;
	BiomeAudio.ReverbAmount = 0.1f;
	BiomeAudio.bMuffledSounds = false;
	BiomeAudio.AmbientCue = TEXT("ambient_aether");
	ActiveHazard = EMistspireBiomeHazard::GravityAnomaly;
	HazardCooldown = 45.f;
}

void UMistspireBiomeAether::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = 0.0f;
	OutWindStrength = FMath::Lerp(50.f, 150.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-50.f, -60.f, AltNormal);
}

void UMistspireBiomeAether::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 1450000.f && Alt < 1580000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown * 0.5f)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::GravityAnomaly;
		}
	}
	else
	{
		HazardTimer = FMath::Max(0.f, HazardTimer - DeltaTime * 2.f);
		if (HazardTimer <= 0.f)
		{
			bHazardActive = false;
		}
	}
}
