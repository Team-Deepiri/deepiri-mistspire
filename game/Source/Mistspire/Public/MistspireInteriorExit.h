#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireInteriorExit.generated.h"

class UBoxComponent;

/** Exit volume placed inside authored interior geometry. */
UCLASS()
class MISTSPIRE_API AMistspireInteriorExit : public AActor
{
	GENERATED_BODY()

public:
	AMistspireInteriorExit();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnExitOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> ExitVolume;
};
