#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireSummitMarker.generated.h"

class UPointLightComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class MISTSPIRE_API AMistspireSummitMarker : public AActor
{
	GENERATED_BODY()
public:
	AMistspireSummitMarker();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") FName SummitId = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float OfficialAltitudeCm = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float ReachRadiusCm = 500.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Summit")
	TObjectPtr<UPointLightComponent> BeaconLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Summit")
	TObjectPtr<UTextRenderComponent> SummitLabel;
};
