#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireInteractionSubsystem.generated.h"

/**
 * Handles proximity-based interaction highlights and sensory feedback.
 */
UCLASS()
class MISTSPIRE_API UMistspireInteractionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void RegisterInteractiveActor(AActor* Actor);
	void UnregisterInteractiveActor(AActor* Actor);

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> InteractiveActors;

	TMap<TObjectPtr<AActor>, bool> HighlightStates;
};
