#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireAltitudeDebugSubsystem.generated.h"

/** On-screen altitude HUD + optional periodic logging (PCVR dev). */
UCLASS()
class MISTSPIRE_API UMistspireAltitudeDebugSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Debug")
	void LogAltitudeStats();

	static bool IsHudEnabled();
	static void SetHudEnabled(bool bEnabled);

private:
	float LogAccumulator = 0.f;
};
