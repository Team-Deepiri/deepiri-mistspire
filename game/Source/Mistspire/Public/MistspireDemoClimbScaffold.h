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
 * Optional Fab env dress uses soft LoadObject paths (safe if packs missing).
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

	/** Soft-ref Fab meshes (Iceland / Rock_04 / Rural Cabin) when present under /Game. */
	UPROPERTY(EditAnywhere, Category = "Mistspire|Demo|EnvDress")
	bool bSpawnEnvDress = true;

	/** Hide Main_WP's OpenWorld-template landscape and leftover authored DemoEnv_* dressing. */
	UPROPERTY(EditAnywhere, Category = "Mistspire|Demo|EnvDress")
	bool bHideTemplateLandscape = true;

	/** Re-aim the map's directional light so the shelf is lit instead of in the summit's shadow. */
	UPROPERTY(EditAnywhere, Category = "Mistspire|Demo|EnvDress")
	bool bAimSunAtVillage = true;

private:
	FTimerHandle LandscapeHideTimer;
	int32 LastHiddenLandscapeCount = -1;

	void ClearBuiltActors();
	void HideTemplateBackdrop();
	void AimSunAtVillage();
	void BuildValley();
	void BuildMistInnPocket();
	void BuildApproachHelix();
	void BuildStation(int32 StationIndex);
	void SpawnImmersionForStation(int32 StationIndex, const FVector& StationLoc);
	void SpawnValleyImmersionProps();

	void ResolveEnvDressMeshes();
	void DressValleyEnv();
	void DressMistInnPorchEnv();
	void DressStationEnv(int32 StationIndex, const FVector& Station, const FVector& RadialOut, const FVector& Tangent, const FRotator& Yaw);

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

	/** Place a resolved env mesh; no tint MID (keeps authored materials). */
	UStaticMeshComponent* AddEnvMesh(
		const FName& NameBase,
		UStaticMesh* Mesh,
		const FVector& WorldLocation,
		const FVector& Scale,
		const FRotator& Rotation,
		bool bCollision = false);

	/**
	 * Place an Iceland terrain patch as a landform, sized and sunk from its own bounds.
	 * DesiredTopZCm is the world Z its highest point lands on, so a massif can never
	 * swallow the village: keep it under the shelf for the summit, above for valley walls.
	 */
	UStaticMeshComponent* AddTerrainMassif(
		const FName& NameBase,
		UStaticMesh* Mesh,
		const FVector2D& OffsetFromValleyCm,
		float FootprintRadiusCm,
		float DesiredTopZCm,
		float YawDeg);

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

	// --- Soft-resolved Fab dress (null entries if pack not in project) ---
	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMesh>> EnvRocks;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMesh>> EnvMountains;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvPine;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvGrass;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvPorch;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvDoor;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvWall;

	/** Wall_Door_4m — solid wall with a cutout; used as the Mist Inn front. */
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvWallDoor;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> EnvRoof;

	bool bEnvDressResolved = false;
};
