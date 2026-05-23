#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireOxygenCanister.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class MISTSPIRE_API AMistspireOxygenCanister : public AActor
{
	GENERATED_BODY()

public:
	AMistspireOxygenCanister();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPickupOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> PickupSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CanisterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Pickup")
	float OxygenRestore = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Pickup")
	bool bConsumeOnPickup = true;
};
