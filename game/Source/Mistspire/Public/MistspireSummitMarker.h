#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireSummitMarker.generated.h"

UCLASS(Blueprintable)
class MISTSPIRE_API AMistspireSummitMarker : public AActor
{
	GENERATED_BODY()
public:
	AMistspireSummitMarker();
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") FName SummitId = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float OfficialAltitudeCm = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float ReachRadiusCm = 500.f;
};
