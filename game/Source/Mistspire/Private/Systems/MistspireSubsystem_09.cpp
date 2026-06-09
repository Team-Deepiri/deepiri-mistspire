#include "Systems/MistspireSubsystem_09.h"

UMistspireBiomeSanctum::UMistspireBiomeSanctum()
{
	MinAltitudeCm = 1600000.f;
	MaxAltitudeCm = 1800000.f;
	VisibilityRangeCm = 10000.f;
	BiomeAudio.AmbientVolume = 0.1f;
	BiomeAudio.WindPitchBias = 0.1f;
	BiomeAudio.ReverbAmount = 0.05f;
	BiomeAudio.bMuffledSounds = true;
	BiomeAudio.AmbientCue = TEXT("ambient_sanctum");
	ActiveHazard = EMistspireBiomeHazard::OxygenVacuum;
	HazardCooldown = 30.f;
}

void UMistspireBiomeSanctum::ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias)
{
	const float Alt = PlayerLocation.Z;
	const float AltNormal = FMath::GetMappedRangeValueClamped(FVector2D(MinAltitudeCm, MaxAltitudeCm), FVector2D(0.f, 1.f), Alt);
	OutMistDensity = 0.0f;
	OutWindStrength = FMath::Lerp(0.f, 30.f, AltNormal);
	OutTemperatureBias = FMath::Lerp(-60.f, -70.f, AltNormal);
}

void UMistspireBiomeSanctum::EvaluateHazards(float DeltaTime, const FVector& PlayerLocation)
{
	const float Alt = PlayerLocation.Z;
	bHazardActive = true;
	ActiveHazard = EMistspireBiomeHazard::OxygenVacuum;
}
