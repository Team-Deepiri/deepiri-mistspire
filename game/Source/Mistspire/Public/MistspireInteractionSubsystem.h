#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireInteractionSubsystem.generated.h"

class AMistspireVRPawn;

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

	/** Non-VR Use (E): trace from pawn camera and call IMistspireInteractable. */
	void TryInteractFromPawn(AMistspireVRPawn* Pawn);

private:
	void GatherProximityPoints(AMistspireVRPawn* Pawn, TArray<FVector>& OutPoints, bool bNonVRMode) const;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> InteractiveActors;

	UPROPERTY()
	TMap<TObjectPtr<AActor>, bool> HighlightStates;
};
