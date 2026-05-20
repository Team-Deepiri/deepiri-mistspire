#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireXRRenderBridge.generated.h"

UCLASS()
class MISTSPIREOPENXRNATIVE_API UMistspireXRRenderBridge : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UFUNCTION(BlueprintCallable) void EnqueueNativeRenderProbe();
	UFUNCTION(BlueprintCallable) bool IsNativeXrReady() const;
private:
	void OnPostEngineInit();
	void ExecuteRenderProbe(FRHICommandListImmediate& RHICmdList);
	bool bHooked = false;
};
