#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireWorldTypes.h"
#include "MistspirePOIMarker.generated.h"

class UTextRenderComponent;
class UPointLightComponent;

UCLASS()
class MISTSPIRE_API AMistspirePOIMarker : public AActor
{
	GENERATED_BODY()

public:
	AMistspirePOIMarker();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|POI")
	FName POIId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|POI")
	EMistspirePOIType POIType = EMistspirePOIType::Landmark;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnDiscoverOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|POI")
	float DiscoverRadiusCm = 400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTextRenderComponent> POILabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPointLightComponent> POILight;

	bool bDiscovered = false;
};
