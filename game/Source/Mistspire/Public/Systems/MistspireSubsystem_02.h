#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_02.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeArid : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeArid();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Arid; }
	virtual FString GetBiomeName() const override { return TEXT("Arid"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.7f, 0.5f, 0.2f); }
};
