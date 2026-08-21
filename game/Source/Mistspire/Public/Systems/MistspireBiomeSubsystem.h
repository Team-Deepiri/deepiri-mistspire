#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireBiomeSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMistspireBiomeType : uint8
{
	None,
	Mist,
	Arid,
	Forest,
	Ember,
	Crystal,
	Void,
	Tundra,
	Aether,
	Sanctum,
	Pinnacle
};

UENUM(BlueprintType)
enum class EMistspireBiomeHazard : uint8
{
	None,
	SiroccoStorm,
	ToxicHaze,
	ResonantFracture,
	LightBlindness,
	MagmaticBurst,
	Hypoxia,
	ThinIce,
	Blizzard,
	GravityAnomaly,
	OxygenVacuum,
	CosmicRadiation
};

USTRUCT(BlueprintType)
struct FMistspireBiomeAudio
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AmbientVolume = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WindPitchBias = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ReverbAmount = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bMuffledSounds = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AmbientCue = TEXT("none");
};

UCLASS(Abstract)
class MISTSPIRE_API UMistspireBiomeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual EMistspireBiomeType GetBiomeType() const PURE_VIRTUAL(UMistspireBiomeSubsystem::GetBiomeType, return EMistspireBiomeType::None;);
	virtual FString GetBiomeName() const PURE_VIRTUAL(UMistspireBiomeSubsystem::GetBiomeName, return TEXT("None"););

	UFUNCTION(BlueprintPure, Category = "Mistspire|Biome")
	float GetMinAltitudeCm() const { return MinAltitudeCm; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Biome")
	float GetMaxAltitudeCm() const { return MaxAltitudeCm; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Biome")
	FMistspireBiomeAudio GetBiomeAudio() const { return BiomeAudio; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Biome")
	float GetVisibilityRangeCm() const { return VisibilityRangeCm; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Biome")
	bool HasActiveHazard() const { return bHazardActive; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Biome")
	EMistspireBiomeHazard GetActiveHazard() const { return ActiveHazard; }

	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) {}
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) {}
	virtual FLinearColor GetBiomeAmbientColor() const { return FLinearColor::White; }

protected:
	UPROPERTY(BlueprintReadOnly) float MinAltitudeCm = 0.f;
	UPROPERTY(BlueprintReadOnly) float MaxAltitudeCm = 1000000.f;
	UPROPERTY(BlueprintReadOnly) float VisibilityRangeCm = 100000.f;
	UPROPERTY(BlueprintReadOnly) FMistspireBiomeAudio BiomeAudio;
	UPROPERTY(BlueprintReadOnly) bool bHazardActive = false;
	UPROPERTY(BlueprintReadOnly) EMistspireBiomeHazard ActiveHazard = EMistspireBiomeHazard::None;
	float HazardTimer = 0.f;
	float HazardCooldown = 60.f;
};
