#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MistspireGameState.generated.h"

/** Replicates summit + altitude stats when multiplayer is enabled later. */
UCLASS()
class MISTSPIRE_API AMistspireGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Score")
	float SessionBestAltitudeCm = 0.f;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Score")
	void NotifyAltitudeSample(float AltitudeCm);
};
