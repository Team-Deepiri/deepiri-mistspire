#include "Systems/MistspireSubsystem_07.h"

UMistspireBiomeTundra::UMistspireBiomeTundra()
{
	MinAltitudeCm = 1200000.f;
	MaxAltitudeCm = 1400000.f;
	VisibilityRangeCm = 30000.f;
	BiomeAudio.AmbientVolume = 0.35f;
	BiomeAudio.WindPitchBias = 2.2f;
	BiomeAudio.ReverbAmount = 0.2f;
	BiomeAudio.bMuffledSounds = true;
	BiomeAudio.AmbientCue = TEXT("ambient_tundra");
	ActiveHazard = EMistspireBiomeHazard::Blizzard;
	HazardCooldown = 60.f;
}

void UMistspireBiomeTundra::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = FMath::Lerp(0.4f, 0.15f, AltNormal);
	OutWindStrength = FMath::Lerp(300.f, 500.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-30.f, -50.f, AltNormal);
}

void UMistspireBiomeTundra::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = false;
	if (Alt > 1250000.f)
	{
		HazardTimer += DeltaTime;
		if (HazardTimer >= HazardCooldown)
		{
			bHazardActive = true;
			ActiveHazard = EMistspireBiomeHazard::Blizzard;
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
