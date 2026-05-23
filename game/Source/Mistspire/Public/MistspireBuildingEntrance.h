#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireBuildingEntrance.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

/** Door volume — walk into the box in VR to enter the building interior. */
UCLASS()
class MISTSPIRE_API AMistspireBuildingEntrance : public AActor
{
	GENERATED_BODY()

public:
	AMistspireBuildingEntrance();

	UFUNCTION(BlueprintPure, Category = "Mistspire|Building")
	FName GetBuildingId() const { return BuildingId; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Building")
	FName BuildingId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Building")
	bool bUseAtlasInteriorSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Building")
	FVector ManualInteriorSpawn = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Building")
	FRotator ManualInteriorRotation = FRotator::ZeroRotator;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDoorOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> DoorVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DoorFrameMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTextRenderComponent> BuildingSign;
};
