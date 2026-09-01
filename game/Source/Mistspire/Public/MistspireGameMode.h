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

	/** When false, atlas door/POI markers are not spawned in non-VR (reduces clutter). */
	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|NonVR")
	bool bSpawnAtlasMarkersInNonVR = false;

protected:
	void SeedDefaultSummits();
	void SeedWorldAtlas();
	void EnsureNonVRPlayground();
	bool HasGroundUnderLocation(const FVector& Location) const;
	FVector ResolveNonVRSpawnLocation() const;
};
