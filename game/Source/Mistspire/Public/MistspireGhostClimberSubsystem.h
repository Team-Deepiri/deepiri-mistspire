#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireGhostClimberSubsystem.generated.h"

class AMistspireGhostPillar;

/** Ethereal pillars showing where other climbers are on the vertical race. */
UCLASS()
class MISTSPIRE_API UMistspireGhostClimberSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

private:
	void RefreshGhosts();

	UPROPERTY()
	TArray<TObjectPtr<AMistspireGhostPillar>> GhostActors;

	TArray<int32> GhostEntityIds;

	float RefreshTimer = 0.f;
};
