#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireLoreShard.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class MISTSPIRE_API AMistspireLoreShard : public AActor
{
	GENERATED_BODY()

public:
	AMistspireLoreShard();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnShardOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Lore")
	FText LoreTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Lore")
	FText LoreBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> ShardSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ShardMesh;
};
