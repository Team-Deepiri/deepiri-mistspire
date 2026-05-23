#include "MistspireZoneSubsystem.h"

EMistspireAltitudeZone UMistspireZoneSubsystem::ZoneFromAltitude(float AltitudeCm)
{
	const float Meters = AltitudeCm / 100.f;
	if (Meters < 500.f) return EMistspireAltitudeZone::Valley;
	if (Meters < 2500.f) return EMistspireAltitudeZone::MistBelt;
	if (Meters < 6000.f) return EMistspireAltitudeZone::Alpine;
	if (Meters < 10000.f) return EMistspireAltitudeZone::ThinAir;
	return EMistspireAltitudeZone::Zenith;
}

FText UMistspireZoneSubsystem::GetZoneDisplayName(EMistspireAltitudeZone Zone)
{
	switch (Zone)
	{
		case EMistspireAltitudeZone::Valley: return NSLOCTEXT("Mistspire", "ZoneValley", "Valley");
		case EMistspireAltitudeZone::MistBelt: return NSLOCTEXT("Mistspire", "ZoneMist", "Mist Belt");
		case EMistspireAltitudeZone::Alpine: return NSLOCTEXT("Mistspire", "ZoneAlpine", "Alpine");
		case EMistspireAltitudeZone::ThinAir: return NSLOCTEXT("Mistspire", "ZoneThinAir", "Thin Air");
		case EMistspireAltitudeZone::Zenith: return NSLOCTEXT("Mistspire", "ZoneZenith", "Zenith");
		default: return FText::GetEmpty();
	}
}

void UMistspireZoneSubsystem::UpdateZoneFromAltitude(float AltitudeCm)
{
	const EMistspireAltitudeZone NewZone = ZoneFromAltitude(AltitudeCm);
	if (NewZone == CurrentZone)
	{
		return;
	}

	const EMistspireAltitudeZone OldZone = CurrentZone;
	CurrentZone = NewZone;
	OnZoneChanged.Broadcast(OldZone, NewZone);
}

float UMistspireZoneSubsystem::GetZoneAmbientIntensity() const
{
	switch (CurrentZone)
	{
		case EMistspireAltitudeZone::Valley: return 0.35f;
		case EMistspireAltitudeZone::MistBelt: return 0.55f;
		case EMistspireAltitudeZone::Alpine: return 0.75f;
		case EMistspireAltitudeZone::ThinAir: return 0.9f;
		case EMistspireAltitudeZone::Zenith: return 1.0f;
		default: return 0.5f;
	}
}
