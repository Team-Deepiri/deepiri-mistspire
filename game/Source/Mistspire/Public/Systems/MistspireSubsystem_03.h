#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_03.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeForest : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeForest();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Forest; }
	virtual FString GetBiomeName() const override { return TEXT("Forest"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.1f, 0.6f, 0.2f); }
};
