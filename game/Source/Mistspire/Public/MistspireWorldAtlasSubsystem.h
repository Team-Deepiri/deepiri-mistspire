#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireWorldTypes.h"
#include "MistspireWorldAtlasSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMistspireDistrictEntered, EMistspireWorldDistrict, OldDistrict, EMistspireWorldDistrict, NewDistrict);

/** Registry for the huge open world: districts, buildings, POIs. */
UCLASS()
class MISTSPIRE_API UMistspireWorldAtlasSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SeedProductionWorld();

	/** Spawns door + POI actors from atlas data (prototype world bootstrap). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|World")
	void SpawnAuthoredWorldMarkers();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|World")
	void RegisterDistrict(const FMistspireDistrictEntry& Entry);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|World")
	void RegisterBuilding(const FMistspireBuildingEntry& Entry);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|World")
	void RegisterPOI(const FMistspirePOIEntry& Entry);

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	EMistspireWorldDistrict GetDistrictAtLocation(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	bool FindBuilding(FName BuildingId, FMistspireBuildingEntry& OutEntry) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	const TArray<FMistspireDistrictEntry>& GetDistricts() const { return Districts; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	const TArray<FMistspireBuildingEntry>& GetBuildings() const { return Buildings; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	const TArray<FMistspirePOIEntry>& GetPOIs() const { return POIs; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	static FText GetDistrictDisplayName(EMistspireWorldDistrict District);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|World")
	void UpdateDistrictFromPlayerLocation(const FVector& WorldLocation);

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	EMistspireWorldDistrict GetCurrentDistrict() const { return CurrentDistrict; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|World")
	float GetWorldRadiusCm() const { return WorldRadiusCm; }

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|World")
	FOnMistspireDistrictEntered OnDistrictEntered;

private:
	UPROPERTY() TArray<FMistspireDistrictEntry> Districts;
	UPROPERTY() TArray<FMistspireBuildingEntry> Buildings;
	UPROPERTY() TArray<FMistspirePOIEntry> POIs;

	EMistspireWorldDistrict CurrentDistrict = EMistspireWorldDistrict::Unknown;
	float WorldRadiusCm = 3500000.f; // ~35 km radius production target
};
