#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireInteriorSubsystem.generated.h"

class AMistspireVRPawn;
class AMistspireBuildingEntrance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMistspireInteriorChanged, FName, BuildingId, bool, bInside);

/** Enter/exit building interiors (VR teleport with return point). */
UCLASS()
class MISTSPIRE_API UMistspireInteriorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Interior")
	bool EnterBuilding(AMistspireVRPawn* Pawn, FName BuildingId, const FVector& ReturnLocation, const FRotator& ReturnRotation);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Interior")
	bool EnterBuildingFromEntrance(AMistspireVRPawn* Pawn, AMistspireBuildingEntrance* Entrance);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Interior")
	void ExitBuilding(AMistspireVRPawn* Pawn);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Interior")
	bool IsInsideInterior() const { return bInsideInterior; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Interior")
	FName GetCurrentBuildingId() const { return CurrentBuildingId; }

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Interior")
	FOnMistspireInteriorChanged OnInteriorChanged;

private:
	bool bInsideInterior = false;
	FName CurrentBuildingId = NAME_None;
	FVector ReturnLocation = FVector::ZeroVector;
	FRotator ReturnRotation = FRotator::ZeroRotator;
};
