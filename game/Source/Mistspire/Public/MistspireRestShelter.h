#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireRestShelter.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPointLightComponent;

/** Oxygen and stamina sanctuary — warm glow, proximity refill, haptic welcome. */
UCLASS()
class MISTSPIRE_API AMistspireRestShelter : public AActor
{
	GENERATED_BODY()

public:
	AMistspireRestShelter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnShelterOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Components")
	TObjectPtr<UBoxComponent> ShelterVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Components")
	TObjectPtr<UStaticMeshComponent> ShelterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Components")
	TObjectPtr<UPointLightComponent> WarmLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float OxygenRefillPerSecond = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float StaminaRefillPerSecond = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float RefillRadiusCm = 250.f;
};
