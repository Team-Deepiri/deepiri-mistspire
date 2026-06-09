#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_10.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomePinnacle : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomePinnacle();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Pinnacle; }
	virtual FString GetBiomeName() const override { return TEXT("Pinnacle"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(1.0f, 0.95f, 0.8f); }
};
