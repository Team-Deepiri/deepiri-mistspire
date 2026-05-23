#include "MistspireSummitRegistry.h"

void UMistspireSummitRegistry::RegisterSummit(FName SummitId, FVector WorldLocation, float OfficialAltitudeCm, float ReachRadiusCm)
{
	for (FMistspireSummitEntry& E : Summits)
	{
		if (E.SummitId == SummitId)
		{
			E.WorldLocation = WorldLocation;
			E.OfficialAltitudeCm = OfficialAltitudeCm;
			E.ReachRadiusCm = ReachRadiusCm;
			return;
		}
	}
	FMistspireSummitEntry N;
	N.SummitId = SummitId;
	N.WorldLocation = WorldLocation;
	N.OfficialAltitudeCm = OfficialAltitudeCm;
	N.ReachRadiusCm = ReachRadiusCm;
	Summits.Add(N);
}

bool UMistspireSummitRegistry::TryReachSummit(FName SummitId, FVector PlayerLocation)
{
	for (FMistspireSummitEntry& E : Summits)
	{
		if (E.SummitId == SummitId && !E.bReached
			&& FVector::DistSquared(PlayerLocation, E.WorldLocation) <= FMath::Square(E.ReachRadiusCm))
		{
			E.bReached = true;
			OnSummitReached.Broadcast(SummitId);
			return true;
		}
	}
	return false;
}

void UMistspireSummitRegistry::ApplyReachedSummits(const TArray<FName>& SummitIds)
{
	for (FMistspireSummitEntry& E : Summits)
	{
		if (SummitIds.Contains(E.SummitId))
		{
			E.bReached = true;
		}
	}
}

FName UMistspireSummitRegistry::GetNextUnreachedSummitId(const TArray<FName>& AlreadyReached) const
{
	float BestDistSq = TNumericLimits<float>::Max();
	FName BestId = NAME_None;

	for (const FMistspireSummitEntry& E : Summits)
	{
		if (E.bReached || AlreadyReached.Contains(E.SummitId))
		{
			continue;
		}
		const float DistSq = E.WorldLocation.SizeSquared();
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestId = E.SummitId;
		}
	}
	return BestId;
}
