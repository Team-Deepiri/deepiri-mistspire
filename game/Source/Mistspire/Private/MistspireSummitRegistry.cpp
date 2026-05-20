#include "MistspireSummitRegistry.h"

void UMistspireSummitRegistry::RegisterSummit(FName SummitId, FVector WorldLocation, float OfficialAltitudeCm)
{
	for (FMistspireSummitEntry& E : Summits)
	{
		if (E.SummitId == SummitId) { E.WorldLocation = WorldLocation; E.OfficialAltitudeCm = OfficialAltitudeCm; return; }
	}
	FMistspireSummitEntry N;
	N.SummitId = SummitId;
	N.WorldLocation = WorldLocation;
	N.OfficialAltitudeCm = OfficialAltitudeCm;
	Summits.Add(N);
}

bool UMistspireSummitRegistry::TryReachSummit(FName SummitId, FVector PlayerLocation, float ReachRadiusCm)
{
	for (FMistspireSummitEntry& E : Summits)
	{
		if (E.SummitId == SummitId && !E.bReached && FVector::DistSquared(PlayerLocation, E.WorldLocation) <= FMath::Square(ReachRadiusCm))
		{
			E.bReached = true;
			return true;
		}
	}
	return false;
}
