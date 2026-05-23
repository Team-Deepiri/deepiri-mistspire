#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "MistspireAmbienceSubsystem.generated.h"

/** Mood layer for audio/post driven by zone + weather (0–1 intensity). */
UCLASS()
class MISTSPIRE_API UMistspireAmbienceSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Ambience")
	float GetTensionLevel() const { return TensionLevel; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Ambience")
	float GetMysteryLevel() const { return MysteryLevel; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Ambience")
	EMistspireAltitudeZone GetDominantZone() const { return DominantZone; }

private:
	float TensionLevel = 0.f;
	float MysteryLevel = 0.f;
	EMistspireAltitudeZone DominantZone = EMistspireAltitudeZone::Valley;
};
