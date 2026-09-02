#include "MistspireLeaderboardService.h"
#include "MistspireGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystemTypes.h"

void UMistspireLeaderboardService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadLocal();
}

void UMistspireLeaderboardService::Deinitialize()
{
	SaveLocal();
	Super::Deinitialize();
}

void UMistspireLeaderboardService::SetBackend(EMistspireLeaderboardBackend InBackend)
{
	Backend = InBackend;
	UE_LOG(LogTemp, Log, TEXT("Mistspire leaderboard backend: %s"),
		Backend == EMistspireLeaderboardBackend::Online ? TEXT("Online") : TEXT("Local"));
}

void UMistspireLeaderboardService::SubmitAltitude(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm)
{
	if (Backend == EMistspireLeaderboardBackend::Online)
	{
		SubmitOnline(PlayerName, CurrentAltitudeCm, MaxAltitudeCm);
	}
	SubmitLocal(PlayerName, CurrentAltitudeCm, MaxAltitudeCm);

	// Persist only when the personal best materially improves, to avoid
	// thrashing the disk on every altitude sample.
	if (MaxAltitudeCm > LastSavedMaxCm + 1000.f)
	{
		LastSavedMaxCm = MaxAltitudeCm;
		SaveLocal();
	}
}

void UMistspireLeaderboardService::SubmitLocal(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm)
{
	FMistspireLeaderboardEntry Entry;
	Entry.PlayerName = PlayerName.IsEmpty() ? TEXT("Local Climber") : PlayerName;
	Entry.CurrentAltitudeCm = CurrentAltitudeCm;
	Entry.MaxAltitudeCm = FMath::Max(MaxAltitudeCm, CurrentAltitudeCm);

	FMistspireLeaderboardEntry* Existing = LocalEntries.FindByPredicate(
		[&Entry](const FMistspireLeaderboardEntry& Other) { return Other.PlayerName == Entry.PlayerName; });
	if (Existing)
	{
		Existing->MaxAltitudeCm = FMath::Max(Existing->MaxAltitudeCm, Entry.MaxAltitudeCm);
		Existing->CurrentAltitudeCm = Entry.CurrentAltitudeCm;
	}
	else
	{
		LocalEntries.Add(Entry);
	}

	LocalEntries.Sort();
	if (LocalEntries.Num() > 64)
	{
		LocalEntries.SetNum(64);
	}

	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
		{
			GS->Leaderboard = LocalEntries;
		}
	}
}

void UMistspireLeaderboardService::SubmitOnline(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Log, TEXT("Mistspire online: no online subsystem available; keeping local."));
		return;
	}

	IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
	if (!Leaderboards.IsValid())
	{
		return;
	}

	ULocalPlayer* LocalPlayer = GEngine ? GEngine->GetFirstGamePlayer(GetGameInstance()->GetWorld()) : nullptr;
	if (!LocalPlayer)
	{
		return;
	}

	const FUniqueNetIdRepl NetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
	const FUniqueNetIdPtr PlayerId = NetIdRepl.IsValid() ? NetIdRepl.GetUniqueNetId() : nullptr;
	if (!PlayerId.IsValid())
	{
		return;
	}

	FOnlineLeaderboardWrite WriteObject;
	WriteObject.LeaderboardNames.Add(TEXT("MistspireAltitude"));
	WriteObject.SetFloatStat(TEXT("AltitudeCm"), MaxAltitudeCm);

	Leaderboards->WriteLeaderboards(TEXT("Mistspire"), *PlayerId, WriteObject);
	Leaderboards->FlushLeaderboards(TEXT("Mistspire"));
}

void UMistspireLeaderboardService::RefreshLeaderboard()
{
	// Local backend: push entries back into the world GameState for ghosts.
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
		{
			GS->Leaderboard = LocalEntries;
		}
	}
}

void UMistspireLeaderboardService::SaveLocal()
{
	if (!SaveData)
	{
		SaveData = NewObject<UMistspireLeaderboardSave>(this);
	}
	SaveData->Entries = LocalEntries;
	UGameplayStatics::SaveGameToSlot(SaveData, TEXT("MistspireLeaderboard"), 0);
}

void UMistspireLeaderboardService::LoadLocal()
{
	if (UMistspireLeaderboardSave* Loaded = Cast<UMistspireLeaderboardSave>(
			UGameplayStatics::LoadGameFromSlot(TEXT("MistspireLeaderboard"), 0)))
	{
		SaveData = Loaded;
		LocalEntries = Loaded->Entries;
		LocalEntries.Sort();
	}
}