#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_01.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeMist : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeMist();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Mist; }
	virtual FString GetBiomeName() const override { return TEXT("Mist"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.4f, 0.5f, 0.6f); }
};
