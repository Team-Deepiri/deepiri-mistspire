#include "MistspireCompanionSubsystem.h"
#include "MistspireGuideSpirit.h"
#include "MistspireBeaconSubsystem.h"
#include "Kismet/GameplayStatics.h"

TStatId UMistspireCompanionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireCompanionSubsystem, STATGROUP_Tickables);
}

void UMistspireCompanionSubsystem::EnsureGuideSpawned()
{
	if (GuideSpirit)
	{
		return;
	}

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Pawn)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GuideSpirit = GetWorld()->SpawnActor<AMistspireGuideSpirit>(Pawn->GetActorLocation(), FRotator::ZeroRotator, Params);
	if (GuideSpirit)
	{
		GuideSpirit->SetFollowTarget(Pawn);
	}
}

void UMistspireCompanionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EnsureGuideSpawned();

	if (!GuideSpirit)
	{
		return;
	}

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Pawn)
	{
		return;
	}

	if (UMistspireBeaconSubsystem* Beacon = GetWorld()->GetSubsystem<UMistspireBeaconSubsystem>())
	{
		const FMistspireBeaconTarget Target = Beacon->GetCachedBeacon();
		if (Target.bValid)
		{
			const FVector Dir = (Target.WorldLocation - Pawn->GetActorLocation()).GetSafeNormal();
			const float Intensity = FMath::Clamp(1.f - (Target.DistanceCm / 500000.f), 0.15f, 1.f);
			GuideSpirit->SetBeaconDirection(Dir, Intensity);
		}
		else
		{
			GuideSpirit->SetBeaconDirection(FVector::UpVector, 1.f);
		}
	}
}
