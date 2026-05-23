#include "MistspireBeaconSubsystem.h"
#include "MistspireSummitRegistry.h"
#include "MistspireVRPawn.h"
#include "MistspirePlayerState.h"
#include "Kismet/GameplayStatics.h"

TStatId UMistspireBeaconSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireBeaconSubsystem, STATGROUP_Tickables);
}

FMistspireBeaconTarget UMistspireBeaconSubsystem::ComputeBeacon(
	const FVector& PlayerLocation, const TArray<FName>& ReachedSummits) const
{
	FMistspireBeaconTarget Best;
	Best.bValid = false;
	float BestDistSq = TNumericLimits<float>::Max();

	if (UMistspireSummitRegistry* Registry = GetWorld()->GetSubsystem<UMistspireSummitRegistry>())
	{
		for (const FMistspireSummitEntry& Entry : Registry->GetSummits())
		{
			if (Entry.bReached || ReachedSummits.Contains(Entry.SummitId))
			{
				continue;
			}

			const float DistSq = FVector::DistSquared(PlayerLocation, Entry.WorldLocation);
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				Best.bValid = true;
				Best.SummitId = Entry.SummitId;
				Best.WorldLocation = Entry.WorldLocation;
				Best.DistanceCm = FMath::Sqrt(DistSq);

				const FVector FlatDelta = Entry.WorldLocation - PlayerLocation;
				Best.BearingDegrees = FMath::RadiansToDegrees(FMath::Atan2(FlatDelta.Y, FlatDelta.X));
			}
		}
	}

	return Best;
}

void UMistspireBeaconSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PulsePhase = FMath::Fmod(PulsePhase + DeltaTime * 2.f, TWO_PI);

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Pawn)
	{
		CachedBeacon = FMistspireBeaconTarget();
		return;
	}

	TArray<FName> Reached;
	if (AMistspirePlayerState* PS = Pawn->GetPlayerState<AMistspirePlayerState>())
	{
		Reached = PS->ReachedSummits;
	}

	CachedBeacon = ComputeBeacon(Pawn->GetActorLocation(), Reached);
}
