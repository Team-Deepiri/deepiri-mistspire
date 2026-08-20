#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "MistspireGameState.h"
#include "MistspireLeaderboardService.generated.h"

UENUM(BlueprintType)
enum class EMistspireLeaderboardBackend : uint8
{
	/** Single-player default: leaderboard persisted to a local save slot. */
	None,
	/** OnlineSubsystem (e.g. Steam) leaderboards; swap in Nakama by implementing one interface. */
	Online,
};

/** Local persistence for the leaderboard service backend. */
UCLASS()
class MISTSPIRE_API UMistspireLeaderboardSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FMistspireLeaderboardEntry> Entries;
};

/**
 * Networking/leaderboard seam (Nakama / Ludus equivalent).
 * Game code talks to this service only; the backend (local, Steam, Nakama)
 * is swappable behind one interface.
 */
UCLASS()
class MISTSPIRE_API UMistspireLeaderboardService : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Online")
	void SetBackend(EMistspireLeaderboardBackend InBackend);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Online")
	EMistspireLeaderboardBackend GetBackend() const { return Backend; }

	/** Submits the local player's altitude to the active backend. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Online")
	void SubmitAltitude(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm);

	/** Reads the backend into the world GameState leaderboard. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Online")
	void RefreshLeaderboard();

	UFUNCTION(BlueprintPure, Category = "Mistspire|Online")
	const TArray<FMistspireLeaderboardEntry>& GetLocalLeaderboard() const { return LocalEntries; }

private:
	void SubmitLocal(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm);
	void SubmitOnline(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm);

	void SaveLocal();
	void LoadLocal();

	EMistspireLeaderboardBackend Backend = EMistspireLeaderboardBackend::None;

	UPROPERTY()
	TObjectPtr<UMistspireLeaderboardSave> SaveData;

	TArray<FMistspireLeaderboardEntry> LocalEntries;

	/** Last persisted max altitude for the current player; throttles disk writes. */
	float LastSavedMaxCm = 0.f;
};