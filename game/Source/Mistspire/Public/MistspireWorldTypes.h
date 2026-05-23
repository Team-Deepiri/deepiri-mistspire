#pragma once

#include "CoreMinimal.h"
#include "MistspireWorldTypes.generated.h"

/** Horizontal districts across the 50 km² production world. */
UENUM(BlueprintType)
enum class EMistspireWorldDistrict : uint8
{
	Unknown,
	ValleyHaven,
	Mistmarket,
	IronMesa,
	CloudPromenade,
	SpireUnderworks,
	RiftQuarter,
	EmberSlums,
	CathedralApproach,
	ObservatoryRing,
	ZenithDock,
	FrostArchive,
	StormBreak,
};

UENUM(BlueprintType)
enum class EMistspirePOIType : uint8
{
	Landmark,
	Lore,
	Shop,
	Shelter,
	Transit,
	Viewpoint
};

USTRUCT(BlueprintType)
struct FMistspireDistrictEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) EMistspireWorldDistrict DistrictId = EMistspireWorldDistrict::Unknown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName DistrictName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector BoundsCenter = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector BoundsExtent = FVector(250000.f, 250000.f, 400000.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MinAltitudeCm = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxAltitudeCm = 1200000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 AuthoredBuildingSlots = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText FlavorLine;
};

USTRUCT(BlueprintType)
struct FMistspireBuildingEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName BuildingId = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName DistrictName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector ExteriorDoorLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator ExteriorDoorRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector InteriorSpawnLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator InteriorSpawnRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText EnterLine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasInterior = true;
};

USTRUCT(BlueprintType)
struct FMistspirePOIEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName POIId = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EMistspirePOIType Type = EMistspirePOIType::Landmark;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
	UPROPERTY(BlueprintReadOnly) bool bDiscovered = false;
};
