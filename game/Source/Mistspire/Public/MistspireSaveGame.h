#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MistspireSaveGame.generated.h"

/** Local progress persisted between sessions. */
UCLASS()
class MISTSPIRE_API UMistspireSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float PersonalBestAltitudeCm = 0.f;

	UPROPERTY()
	TArray<FName> ReachedSummits;

	UPROPERTY()
	int32 TotalSessions = 0;
};
