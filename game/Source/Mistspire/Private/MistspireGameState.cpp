#include "MistspireGameState.h"
#include "MistspirePlayerState.h"
#include "MistspireLeaderboardService.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"

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

void AMistspireGameState::NotifyAltitudeSample(float CurrentAltitudeCm, float MaxAltitudeCm)
{
	if (!HasAuthority()) return;
	if (AMistspirePlayerState* PS = GetPlayerState<AMistspirePlayerState>())
	{
		NotifyAltitudeSample(PS->GetPlayerName(), CurrentAltitudeCm, MaxAltitudeCm);
	}
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

	// Persist through the pluggable leaderboard service (local save / online backend).
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMistspireLeaderboardService* Service = GameInstance->GetSubsystem<UMistspireLeaderboardService>())
		{
			Service->SubmitAltitude(PlayerName, CurrentAltitudeCm, MaxAltitudeCm);
		}
	}
}

void AMistspireGameState::BroadcastSocialAchievement(const FString& Message)
{
	if (!HasAuthority()) return;
	UE_LOG(LogTemp, Log, TEXT("Mistspire Social: %s"), *Message);
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			PC->ClientMessage(Message);
		}
	}
}
