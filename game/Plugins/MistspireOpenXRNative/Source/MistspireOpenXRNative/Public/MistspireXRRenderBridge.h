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
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|XR")
	void EnqueueNativeRenderProbe();

	UFUNCTION(BlueprintPure, Category = "Mistspire|XR")
	bool IsNativeXrReady() const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|XR")
	int64 GetPreferredSwapchainFormat() const { return PreferredSwapchainFormat; }

private:
	void OnPostEngineInit();
	void ExecuteRenderProbe(FRHICommandListImmediate& RHICmdList);
	void EnqueueViewProbe();
	uint64 GetXrSystemId() const;
	void DestroySwapchain();

	bool bHooked = false;
	bool bNativeXrReady = false;
	int64 PreferredSwapchainFormat = 0;
};
