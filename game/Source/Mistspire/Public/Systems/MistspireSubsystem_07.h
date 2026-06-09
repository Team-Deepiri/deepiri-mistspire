#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_07.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeTundra : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeTundra();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Tundra; }
	virtual FString GetBiomeName() const override { return TEXT("Tundra"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.9f, 0.9f, 1.0f); }
};
