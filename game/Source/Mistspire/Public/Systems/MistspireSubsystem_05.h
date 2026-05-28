#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_05.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeCrystal : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeCrystal();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Crystal; }
	virtual FString GetBiomeName() const override { return TEXT("Crystal"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.2f, 0.8f, 1.0f); }
};
