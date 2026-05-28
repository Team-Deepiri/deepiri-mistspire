#include "Systems/MistspireSubsystem_06.h"

UMistspireBiomeVoid::UMistspireBiomeVoid()
{
	MinAltitudeCm = 900000.f;
	MaxAltitudeCm = 1200000.f;
	VisibilityRangeCm = 150000.f;
	BiomeAudio.AmbientVolume = 0.2f;
	BiomeAudio.WindPitchBias = 2.0f;
	BiomeAudio.ReverbAmount = 0.3f;
	BiomeAudio.bMuffledSounds = true;
	BiomeAudio.AmbientCue = TEXT("ambient_void");
	ActiveHazard = EMistspireBiomeHazard::LightBlindness;
	HazardCooldown = 60.f;
}

void UMistspireBiomeVoid::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = 0.01f;
	OutWindStrength = FMath::Lerp(200.f, 400.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-30.f, -60.f, AltNormal);
}

void UMistspireBiomeVoid::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 950000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::LightBlindness;
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
