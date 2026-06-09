#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_08.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeAether : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeAether();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Aether; }
	virtual FString GetBiomeName() const override { return TEXT("Aether"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.6f, 0.2f, 1.0f); }
};
