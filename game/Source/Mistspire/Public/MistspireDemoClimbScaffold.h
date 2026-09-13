#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspireDemoClimbScaffold.generated.h"

class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
 * Runtime Demo Spire geometry + immersion props for -demoworld recordings.
 * Spawns a helix of biome stations with local climb vignettes.
 */
UCLASS()
class MISTSPIRE_API AMistspireDemoClimbScaffold : public AActor
{
	GENERATED_BODY()

public:
	AMistspireDemoClimbScaffold();

	virtual void BeginPlay() override;

	/** Destroy child meshes/props and rebuild the full spire. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Demo")
	void Rebuild();

	/** Find existing scaffold or spawn one, then Rebuild. */
	static AMistspireDemoClimbScaffold* EnsureInWorld(UWorld* World);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Demo")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, Category = "Mistspire|Demo")
	bool bSpawnImmersionProps = true;

	UPROPERTY(EditAnywhere, Category = "Mistspire|Demo")
	bool bSpawnApproachHelix = true;

	UPROPERTY(EditAnywhere, Category = "Mistspire|Demo")
	bool bSpawnDistantSilhouettes = true;

private:
	void ClearBuiltActors();
	void BuildValley();
	void BuildMistInnPocket();
	void BuildApproachHelix();
	void BuildStation(int32 StationIndex);
	void BuildDistantSilhouettes();
	void SpawnImmersionForStation(int32 StationIndex, const FVector& StationLoc);
	void SpawnValleyImmersionProps();

	UStaticMeshComponent* AddCube(
		const FName& NameBase,
		const FVector& WorldLocation,
		const FVector& Scale100cm,
		const FRotator& Rotation,
		const FLinearColor& Tint,
		bool bCollision = true);

	UStaticMeshComponent* AddCylinder(
		const FName& NameBase,
		const FVector& WorldLocation,
		const FVector& Scale100cm,
		const FRotator& Rotation,
		const FLinearColor& Tint,
		bool bCollision = true);

	void BuildGrappleShaftToMist();
	void BuildCentralMast();

	UMaterialInstanceDynamic* GetOrCreateTintMID(const FLinearColor& Tint);

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedPropActors;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> CachedTintMIDs;
};
