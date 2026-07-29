#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MistspireGameInstance.generated.h"

UCLASS()
class MISTSPIRE_API UMistspireGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMistspireGameInstance();

	virtual void Init() override;

	/** Starts a hosting session. bIsLAN determines if it's visible on the local network. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Multiplayer")
	void HostSession(bool bIsLAN = false, int32 MaxPlayers = 4);

	/** Starts searching for available sessions. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Multiplayer")
	void FindSessions();

	/** Joins a specific session found via FindSessions. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Multiplayer")
	void JoinFoundSession(int32 SessionIndex);

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
};
