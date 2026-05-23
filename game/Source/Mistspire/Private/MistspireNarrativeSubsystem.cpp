#include "MistspireNarrativeSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireSummitRegistry.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireGameState.h"
#include "Engine/Engine.h"

void UMistspireNarrativeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UMistspireZoneSubsystem* Zone = GetWorld()->GetSubsystem<UMistspireZoneSubsystem>())
	{
		ZoneHandle = Zone->OnZoneChanged.AddUObject(this, &UMistspireNarrativeSubsystem::HandleZoneChanged);
	}

	if (UMistspireAltitudeSubsystem* Alt = GetWorld()->GetSubsystem<UMistspireAltitudeSubsystem>())
	{
		AltitudeHandle = Alt->OnAltitudeRecord.AddUObject(this, &UMistspireNarrativeSubsystem::HandleAltitudeRecord);
	}

	if (UMistspireSummitRegistry* Registry = GetWorld()->GetSubsystem<UMistspireSummitRegistry>())
	{
		Registry->OnSummitReached.AddUObject(this, &UMistspireNarrativeSubsystem::OnSummitReached);
	}

	if (UMistspireWorldAtlasSubsystem* Atlas = GetWorld()->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		DistrictHandle = Atlas->OnDistrictEntered.AddUObject(this, &UMistspireNarrativeSubsystem::HandleDistrictChanged);
	}
}

void UMistspireNarrativeSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		if (UMistspireZoneSubsystem* Zone = World->GetSubsystem<UMistspireZoneSubsystem>())
		{
			Zone->OnZoneChanged.Remove(ZoneHandle);
		}
		if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
		{
			Alt->OnAltitudeRecord.Remove(AltitudeHandle);
		}
		if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
		{
			Atlas->OnDistrictEntered.Remove(DistrictHandle);
		}
	}
	Super::Deinitialize();
}

void UMistspireNarrativeSubsystem::PushLine(const FText& Line, float DisplaySeconds)
{
	LastLine = Line;
	OnNarrativeLine.Broadcast(Line);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, DisplaySeconds, FColor::Silver,
			FString::Printf(TEXT("◆ %s"), *Line.ToString()));
	}
}

void UMistspireNarrativeSubsystem::HandleZoneChanged(EMistspireAltitudeZone OldZone, EMistspireAltitudeZone NewZone)
{
	switch (NewZone)
	{
		case EMistspireAltitudeZone::MistBelt:
			PushLine(NSLOCTEXT("Mistspire", "EnterMist", "The mist thickens. Grip holds like stone."));
			break;
		case EMistspireAltitudeZone::Alpine:
			PushLine(NSLOCTEXT("Mistspire", "EnterAlpine", "Wind claws the ridge. Your lungs work harder."));
			break;
		case EMistspireAltitudeZone::ThinAir:
			PushLine(NSLOCTEXT("Mistspire", "EnterThinAir", "Thin air. Vision swims. Oxygen is life."));
			break;
		case EMistspireAltitudeZone::Zenith:
			PushLine(NSLOCTEXT("Mistspire", "EnterZenith", "Zenith. The world falls away beneath you."));
			break;
		default:
			break;
	}
}

void UMistspireNarrativeSubsystem::HandleAltitudeRecord(float NewAltitudeCm, float PersonalBestCm)
{
	const int32 Km = FMath::FloorToInt(PersonalBestCm / 100000.f);
	if (Km <= 0 || Km == LastMilestoneKm)
	{
		return;
	}
	LastMilestoneKm = Km;
	PushLine(FText::Format(
		NSLOCTEXT("Mistspire", "AltMilestone", "Personal best: {0} km above the valley."),
		FText::AsNumber(Km)));
}

void UMistspireNarrativeSubsystem::OnSummitReached(FName SummitId)
{
	FString SummitLabel = SummitId.ToString();
	SummitLabel.RemoveFromStart(TEXT("summit_"));
	SummitLabel.ReplaceInline(TEXT("_"), TEXT(" "));

	PushLine(FText::Format(
		NSLOCTEXT("Mistspire", "SummitReached", "Summit claimed: {0}."),
		FText::FromString(SummitLabel)), 8.f);

	if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
	{
		GS->BroadcastSocialAchievement(FString::Printf(TEXT("reached %s"), *SummitLabel));
	}
}

void UMistspireNarrativeSubsystem::OnBuildingEntered(FName BuildingId, FText DisplayName)
{
	PushLine(FText::Format(
		NSLOCTEXT("Mistspire", "BuildingEntered", "Inside: {0}"),
		DisplayName), 5.f);
}

void UMistspireNarrativeSubsystem::OnPOIDiscovered(const FMistspirePOIEntry& POI)
{
	PushLine(FText::Format(
		NSLOCTEXT("Mistspire", "POIFound", "Discovered: {0}"),
		POI.Title), 6.f);
	if (!POI.Description.IsEmpty())
	{
		PushLine(POI.Description, 7.f);
	}
}

void UMistspireNarrativeSubsystem::HandleDistrictChanged(EMistspireWorldDistrict OldDistrict, EMistspireWorldDistrict NewDistrict)
{
	if (NewDistrict == EMistspireWorldDistrict::Unknown)
	{
		return;
	}

	if (UMistspireWorldAtlasSubsystem* Atlas = GetWorld()->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		for (const FMistspireDistrictEntry& D : Atlas->GetDistricts())
		{
			if (D.DistrictId == NewDistrict && !D.FlavorLine.IsEmpty())
			{
				PushLine(FText::Format(
					NSLOCTEXT("Mistspire", "DistrictEnter", "{0} — {1}"),
					UMistspireWorldAtlasSubsystem::GetDistrictDisplayName(NewDistrict),
					D.FlavorLine), 7.f);
				return;
			}
		}
	}

	PushLine(FText::Format(
		NSLOCTEXT("Mistspire", "DistrictEnterShort", "Entering {0}."),
		UMistspireWorldAtlasSubsystem::GetDistrictDisplayName(NewDistrict)), 5.f);
}
