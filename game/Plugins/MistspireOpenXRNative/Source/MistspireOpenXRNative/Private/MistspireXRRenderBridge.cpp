#include "MistspireXRRenderBridge.h"
#include "MistspireOpenXRAccess.h"
#include "RenderingThread.h"
#include "RHICommandList.h"

void UMistspireXRRenderBridge::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (!bHooked) { FCoreDelegates::OnPostEngineInit.AddUObject(this, &UMistspireXRRenderBridge::OnPostEngineInit); bHooked = true; }
}
void UMistspireXRRenderBridge::OnPostEngineInit() { EnqueueNativeRenderProbe(); }
bool UMistspireXRRenderBridge::IsNativeXrReady() const { return FMistspireOpenXRAccess::IsOpenXRAvailable(); }
void UMistspireXRRenderBridge::EnqueueNativeRenderProbe()
{
	ENQUEUE_RENDER_COMMAND(MistspireXRProbe)([this](FRHICommandListImmediate& R) { ExecuteRenderProbe(R); });
}
void UMistspireXRRenderBridge::ExecuteRenderProbe(FRHICommandListImmediate& R)
{
	(void)R.GetNativeDevice();
	XrInstance I = nullptr; XrSession S = nullptr;
	(void)FMistspireOpenXRAccess::GetNativeHandles(I, S);
}
