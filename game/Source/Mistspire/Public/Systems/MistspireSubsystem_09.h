#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_09.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeSanctum : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeSanctum();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Sanctum; }
	virtual FString GetBiomeName() const override { return TEXT("Sanctum"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.1f, 0.0f, 0.3f); }
};
