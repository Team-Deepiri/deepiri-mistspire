#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "MistspireWorldTypes.h"
#include "MistspireNarrativeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMistspireNarrativeLine, FText, Line);

/** Delivers altitude-zone whispers, summit fanfare, and milestone callouts. */
UCLASS()
class MISTSPIRE_API UMistspireNarrativeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Narrative")
	void PushLine(const FText& Line, float DisplaySeconds = 6.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Narrative")
	void OnSummitReached(FName SummitId);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Narrative")
	void OnBuildingEntered(FName BuildingId, FText DisplayName);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Narrative")
	void OnPOIDiscovered(const FMistspirePOIEntry& POI);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Narrative")
	FText GetLastLine() const { return LastLine; }

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Narrative")
	FOnMistspireNarrativeLine OnNarrativeLine;

private:
	UFUNCTION()
	void HandleZoneChanged(EMistspireAltitudeZone OldZone, EMistspireAltitudeZone NewZone);

	UFUNCTION()
	void HandleAltitudeRecord(float NewAltitudeCm, float PersonalBestCm);

	UFUNCTION()
	void HandleDistrictChanged(EMistspireWorldDistrict OldDistrict, EMistspireWorldDistrict NewDistrict);

	FText LastLine;
	int32 LastMilestoneKm = -1;
};
