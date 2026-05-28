#pragma once
#include "CoreMinimal.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireSubsystem_06.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireBiomeVoid : public UMistspireBiomeSubsystem
{
	GENERATED_BODY()
public:
	UMistspireBiomeVoid();
	virtual EMistspireBiomeType GetBiomeType() const override { return EMistspireBiomeType::Void; }
	virtual FString GetBiomeName() const override { return TEXT("Void"); }
	virtual void ApplyBiomeEffects(float DeltaTime, const FVector& PlayerLocation, float& OutWindStrength, float& OutMistDensity, float& OutTemperatureBias) override;
	virtual void EvaluateHazards(float DeltaTime, const FVector& PlayerLocation) override;
	virtual FLinearColor GetBiomeAmbientColor() const override { return FLinearColor(0.05f, 0.05f, 0.15f); }
};
