#include "MistspireGameState.h"
#include "Net/UnrealNetwork.h"

AMistspireGameState::AMistspireGameState()
{
	bReplicates = true;
}

void AMistspireGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const
{
	Super::GetLifetimeReplicatedProps(OutReplicatedProps);

	DOREPLIFETIME(AMistspireGameState, SessionBestAltitudeCm);
	DOREPLIFETIME(AMistspireGameState, Leaderboard);
	DOREPLIFETIME(AMistspireGameState, CurrentWeatherIndex);
}

void AMistspireGameState::NotifyAltitudeSample(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm)
{
	if (!HasAuthority()) return;

	if (MaxAltitudeCm > SessionBestAltitudeCm)
	{
		SessionBestAltitudeCm = MaxAltitudeCm;
	}

	// Update or Add leaderboard entry
	FMistspireLeaderboardEntry* Entry = Leaderboard.FindByPredicate([&](const FMistspireLeaderboardEntry& E) {
		return E.PlayerName == PlayerName;
	});

	if (Entry)
	{
		Entry->CurrentAltitudeCm = CurrentAltitudeCm;
		Entry->MaxAltitudeCm = MaxAltitudeCm;
	}
	else
	{
		FMistspireLeaderboardEntry NewEntry;
		NewEntry.PlayerName = PlayerName;
		NewEntry.CurrentAltitudeCm = CurrentAltitudeCm;
		NewEntry.MaxAltitudeCm = MaxAltitudeCm;
		Leaderboard.Add(NewEntry);
	}

	// Sort leaderboard periodically or on significant change
	Leaderboard.Sort();
}

void AMistspireGameState::BroadcastSocialAchievement(const FString& Message)
{
	if (!HasAuthority()) return;
	// Logic to send to all player UIs
	UE_LOG(LogTemp, Log, TEXT("Mistspire Social: %s"), *Message);
}
