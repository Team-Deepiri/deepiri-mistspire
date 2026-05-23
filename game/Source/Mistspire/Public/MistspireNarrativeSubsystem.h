#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireZoneSubsystem.h"
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

	UFUNCTION(BlueprintPure, Category = "Mistspire|Narrative")
	FText GetLastLine() const { return LastLine; }

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Narrative")
	FOnMistspireNarrativeLine OnNarrativeLine;

private:
	void HandleZoneChanged(EMistspireAltitudeZone OldZone, EMistspireAltitudeZone NewZone);
	void HandleAltitudeRecord(float NewAltitudeCm, float PersonalBestCm);

	FText LastLine;
	int32 LastMilestoneKm = -1;

	FDelegateHandle ZoneHandle;
	FDelegateHandle AltitudeHandle;
};
