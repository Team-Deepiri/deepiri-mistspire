#include "MistspireGhostClimberSubsystem.h"
#include "MistspireGhostPillar.h"
#include "MistspireGameState.h"
#include "MistspireEntitySubsystem.h"
#include "Kismet/GameplayStatics.h"

TStatId UMistspireGhostClimberSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireGhostClimberSubsystem, STATGROUP_Tickables);
}

void UMistspireGhostClimberSubsystem::RefreshGhosts()
{
	for (AMistspireGhostPillar* Pillar : GhostActors)
	{
		if (Pillar)
		{
			Pillar->Destroy();
		}
	}
	GhostActors.Reset();

	APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>();
	if (!LocalPawn || !GS)
	{
		return;
	}

	const FVector PlayerLoc = LocalPawn->GetActorLocation();
	int32 Spawned = 0;

	for (const FMistspireLeaderboardEntry& Entry : GS->Leaderboard)
	{
		if (Spawned >= 6 || Entry.PlayerName.IsEmpty())
		{
			continue;
		}

		const FVector GhostBase(
			PlayerLoc.X + FMath::Sin(Spawned * 1.9f) * 1200.f,
			PlayerLoc.Y + FMath::Cos(Spawned * 1.9f) * 1200.f,
			0.f);

		if (FVector::DistSquared2D(GhostBase, PlayerLoc) > FMath::Square(250000.f))
		{
			continue;
		}

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AMistspireGhostPillar* Pillar = GetWorld()->SpawnActor<AMistspireGhostPillar>(GhostBase, FRotator::ZeroRotator, Params);
		if (Pillar)
		{
			Pillar->Configure(FVector(GhostBase.X, GhostBase.Y, Entry.CurrentAltitudeCm), Entry.CurrentAltitudeCm, Entry.PlayerName);
			GhostActors.Add(Pillar);
			++Spawned;

			// Register in the entity store so AI/RL systems can query climber ghosts.
			if (UMistspireEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMistspireEntitySubsystem>())
			{
				const int32 EntityId = Entities->SpawnEntity(TEXT("GhostPillar"), Pillar->GetActorLocation(), Pillar);
				if (EntityId != INDEX_NONE)
				{
					Entities->SetEntityFloat(EntityId, TEXT("AltitudeCm"), Entry.CurrentAltitudeCm);
					Entities->SetEntityTag(EntityId, TEXT("ClimberName"), FName(*Entry.PlayerName));
				}
			}
		}
	}
}

void UMistspireGhostClimberSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RefreshTimer += DeltaTime;
	if (RefreshTimer >= 2.5f)
	{
		RefreshTimer = 0.f;
		RefreshGhosts();
	}
}
