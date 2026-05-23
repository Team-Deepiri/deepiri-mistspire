#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireWindCrystal.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class MISTSPIRE_API AMistspireWindCrystal : public AActor
{
	GENERATED_BODY()

public:
	AMistspireWindCrystal();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCrystalOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> CrystalSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CrystalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Pickup")
	float GliderBoostSeconds = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Pickup")
	float StaminaRestore = 20.f;
};
