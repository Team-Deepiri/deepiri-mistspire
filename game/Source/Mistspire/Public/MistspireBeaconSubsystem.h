#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireBeaconSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMistspireBeaconTarget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName SummitId = NAME_None;
	UPROPERTY(BlueprintReadOnly) FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly) float DistanceCm = 0.f;
	UPROPERTY(BlueprintReadOnly) float BearingDegrees = 0.f;
	UPROPERTY(BlueprintReadOnly) bool bValid = false;
};

/** Points the climber toward the next unreached summit with bearing + pulse cues. */
UCLASS()
class MISTSPIRE_API UMistspireBeaconSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Beacon")
	FMistspireBeaconTarget ComputeBeacon(const FVector& PlayerLocation, const TArray<FName>& ReachedSummits) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Beacon")
	FMistspireBeaconTarget GetCachedBeacon() const { return CachedBeacon; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Beacon")
	float GetPulsePhase() const { return PulsePhase; }

private:
	FMistspireBeaconTarget CachedBeacon;
	float PulsePhase = 0.f;
};
