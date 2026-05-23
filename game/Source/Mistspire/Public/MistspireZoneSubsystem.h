#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireZoneSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMistspireAltitudeZone : uint8
{
	Valley,
	MistBelt,
	Alpine,
	ThinAir,
	Zenith
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMistspireZoneChanged, EMistspireAltitudeZone, OldZone, EMistspireAltitudeZone, NewZone);

/** Altitude strata for atmosphere, narrative, and gameplay tuning. */
UCLASS()
class MISTSPIRE_API UMistspireZoneSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Zone")
	void UpdateZoneFromAltitude(float AltitudeCm);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Zone")
	EMistspireAltitudeZone GetCurrentZone() const { return CurrentZone; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Zone")
	static EMistspireAltitudeZone ZoneFromAltitude(float AltitudeCm);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Zone")
	static FText GetZoneDisplayName(EMistspireAltitudeZone Zone);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Zone")
	float GetZoneAmbientIntensity() const;

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Zone")
	FOnMistspireZoneChanged OnZoneChanged;

private:
	EMistspireAltitudeZone CurrentZone = EMistspireAltitudeZone::Valley;
};
