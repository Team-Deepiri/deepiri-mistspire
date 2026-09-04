#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireWanderingGhost.generated.h"

class UStaticMeshComponent;
class UMistspireSteeringComponent;
class UMistspireStateMachineComponent;

/**
 * Self-contained AI demonstrator: steering behaviors + state chart + entity
 * store registration. Spawn with the `mistspire.SpawnGhostSim` console command.
 */
UCLASS()
class MISTSPIRE_API AMistspireWanderingGhost : public AActor
{
	GENERATED_BODY()

public:
	AMistspireWanderingGhost();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|AI")
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|AI")
	TObjectPtr<UMistspireSteeringComponent> Steering;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|AI")
	TObjectPtr<UMistspireStateMachineComponent> StateMachine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI")
	FName EntityArchetype = TEXT("Ghost");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI")
	float BehaviorChangeIntervalSeconds = 6.f;

private:
	void PickNextBehavior();

	UFUNCTION()
	void OnGhostStateChanged(FName OldState, FName NewState);

	int32 EntityId = INDEX_NONE;
	float BehaviorTimer = 0.f;
};