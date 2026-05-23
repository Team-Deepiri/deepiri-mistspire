#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireGhostPillar.generated.h"

class UStaticMeshComponent;

/** Vertical marker showing another climber's altitude in multiplayer. */
UCLASS()
class MISTSPIRE_API AMistspireGhostPillar : public AActor
{
	GENERATED_BODY()

public:
	AMistspireGhostPillar();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Ghost")
	void Configure(const FVector& WorldLocation, float HeightCm, const FString& Label);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> PillarMesh;
};
