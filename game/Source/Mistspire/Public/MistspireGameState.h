#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MistspireGameState.generated.h"

USTRUCT(BlueprintType)
struct FMistspireLeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	float MaxAltitudeCm = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentAltitudeCm = 0.f;

	bool operator<(const FMistspireLeaderboardEntry& Other) const
	{
		return MaxAltitudeCm > Other.MaxAltitudeCm; // Sort descending
	}
};

/** Manages global session state and leaderboard for up to 30 players. */
UCLASS()
class MISTSPIRE_API AMistspireGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMistspireGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Score")
	float SessionBestAltitudeCm = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Score")
	TArray<FMistspireLeaderboardEntry> Leaderboard;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Score")
	void NotifyAltitudeSample(const FString& PlayerName, float CurrentAltitudeCm, float MaxAltitudeCm);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Score")
	void BroadcastSocialAchievement(const FString& Message);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Environment")
	uint8 CurrentWeatherIndex = 0; // Sync weather for all players
};
