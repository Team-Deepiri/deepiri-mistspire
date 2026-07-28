#include "MistspireXRRenderBridge.h"
#include "MistspireOpenXRAccess.h"
#include "RenderingThread.h"
#include "RHICommandList.h"
#include "DynamicRHI.h"
#include "Engine/Engine.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IOpenXRHMD.h"
#include "IOpenXRHMDModule.h"

namespace MistspireXRRenderBridgePrivate
{
	constexpr uint64 NullSystemId = 0;
}

void UMistspireXRRenderBridge::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (!bHooked)
	{
		FCoreDelegates::OnPostEngineInit.AddUObject(this, &UMistspireXRRenderBridge::OnPostEngineInit);
		bHooked = true;
	}
}

void UMistspireXRRenderBridge::Deinitialize()
{
	DestroySwapchain();
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	Super::Deinitialize();
}

bool UMistspireXRRenderBridge::IsNativeXrReady() const
{
	return bNativeXrReady && FMistspireOpenXRAccess::IsOpenXRAvailable();
}

void UMistspireXRRenderBridge::OnPostEngineInit()
{
	EnqueueNativeRenderProbe();
}

void UMistspireXRRenderBridge::EnqueueNativeRenderProbe()
{
	ENQUEUE_RENDER_COMMAND(MistspireXRProbe)(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			ExecuteRenderProbe(RHICmdList);
		}
	);
}

void UMistspireXRRenderBridge::ExecuteRenderProbe(FRHICommandListImmediate& RHICmdList)
{
	void* NativeDevice = RHICmdList.GetNativeDevice();
	if (!NativeDevice)
	{
		UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: No native device"));
		return;
	}

	XrInstance Instance = XR_NULL_HANDLE;
	XrSession Session = XR_NULL_HANDLE;
	if (!FMistspireOpenXRAccess::GetNativeHandles(Instance, Session))
	{
		UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: OpenXR not available"));
		return;
	}

	IXRTrackingSystem* XRTracking = GEngine ? GEngine->XRSystem.Get() : nullptr;
	if (!XRTracking)
	{
		UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: No XR tracking system"));
		return;
	}

	IOpenXRHMD* OpenXRHMD = XRTracking->GetIOpenXRHMD();
	if (!OpenXRHMD || !OpenXRHMD->IsInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: OpenXRHMD not initialized"));
		return;
	}

	IHeadMountedDisplay* HMD = XRTracking->GetHMDDevice();
	if (!HMD)
	{
		UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: No HMD device"));
		return;
	}

	// Soft probe: UE owns the swapchain; we only confirm native handles + RHI are live.
	bNativeXrReady = true;
	PreferredSwapchainFormat = 0;

	const FString RHIName = GDynamicRHI ? GDynamicRHI->GetName() : TEXT("None");
	UE_LOG(LogTemp, Log, TEXT("MistspireXRRenderBridge: Ready — RHI=%s, Instance=0x%p, Session=0x%p, SystemId=%llu, device=0x%p"),
		*RHIName, Instance, Session, static_cast<uint64>(OpenXRHMD->GetSystem()), NativeDevice);

	FFunctionGraphTask::CreateAndDispatchWhenReady(
		[this]()
		{
			EnqueueViewProbe();
		},
		TStatId(), nullptr, ENamedThreads::GameThread);
}

void UMistspireXRRenderBridge::EnqueueViewProbe()
{
	const uint64 SystemId = GetXrSystemId();
	UE_LOG(LogTemp, Verbose, TEXT("MistspireXRRenderBridge: View probe — SystemId=%llu"), SystemId);
}

uint64 UMistspireXRRenderBridge::GetXrSystemId() const
{
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		if (IOpenXRHMD* OpenXRHMD = GEngine->XRSystem->GetIOpenXRHMD())
		{
			return static_cast<uint64>(OpenXRHMD->GetSystem());
		}
	}

	if (IOpenXRHMDModule::IsAvailable())
	{
		return static_cast<uint64>(IOpenXRHMDModule::Get().GetSystemId());
	}

	return MistspireXRRenderBridgePrivate::NullSystemId;
}

void UMistspireXRRenderBridge::DestroySwapchain()
{
	bNativeXrReady = false;
	PreferredSwapchainFormat = 0;
}
