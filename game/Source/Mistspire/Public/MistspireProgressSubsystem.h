#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MistspireProgressSubsystem.generated.h"

/** Loads and saves climber progress (altitude PB, summits). */
UCLASS()
class MISTSPIRE_API UMistspireProgressSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static const FString SaveSlotName;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Progress")
	void LoadProgress();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Progress")
	void SaveProgress();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Progress")
	void ApplyLoadedProgressToWorld(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Progress")
	void CaptureProgressFromWorld(UWorld* World);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Progress")
	float GetCachedPersonalBestCm() const { return CachedPersonalBestCm; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Progress")
	const TArray<FName>& GetCachedSummits() const { return CachedSummits; }

private:
	float CachedPersonalBestCm = 0.f;
	TArray<FName> CachedSummits;
	bool bLoaded = false;
};
