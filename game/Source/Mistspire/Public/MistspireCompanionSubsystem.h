#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireCompanionSubsystem.generated.h"

class AMistspireGuideSpirit;

/** Spawns and drives the local guide spirit orb. */
UCLASS()
class MISTSPIRE_API UMistspireCompanionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Companion")
	AMistspireGuideSpirit* GetGuideSpirit() const { return GuideSpirit; }

private:
	void EnsureGuideSpawned();

	UPROPERTY()
	TObjectPtr<AMistspireGuideSpirit> GuideSpirit;
};
