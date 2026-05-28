#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_04.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeEmber : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeEmber();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Ember; }
	virtual FString GetBiomeName() const override { return TEXT("Ember"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.8f, 0.15f, 0.05f); }
};
