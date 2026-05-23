#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireGuideSpirit.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;

/** Floating companion that drifts toward the next summit beacon. */
UCLASS()
class MISTSPIRE_API AMistspireGuideSpirit : public AActor
{
	GENERATED_BODY()

public:
	AMistspireGuideSpirit();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Guide")
	void SetFollowTarget(AActor* InTarget);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Guide")
	void SetBeaconDirection(const FVector& WorldDirection, float Intensity01);

protected:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> OrbMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPointLightComponent> GlowLight;

	UPROPERTY(EditAnywhere, Category = "Mistspire|Guide")
	float OrbitRadiusCm = 55.f;

	UPROPERTY(EditAnywhere, Category = "Mistspire|Guide")
	float FollowSpeed = 420.f;

private:
	TWeakObjectPtr<AActor> FollowTarget;
	FVector BeaconDirection = FVector::ForwardVector;
	float BeaconIntensity = 0.f;
	float OrbitAngle = 0.f;
};
