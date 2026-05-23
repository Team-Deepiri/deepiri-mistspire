#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MistspireGameMode.generated.h"

UCLASS()
class MISTSPIRE_API AMistspireGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMistspireGameMode();

	virtual void StartPlay() override;

protected:
	void SeedDefaultSummits();
	void SeedWorldAtlas();
};
