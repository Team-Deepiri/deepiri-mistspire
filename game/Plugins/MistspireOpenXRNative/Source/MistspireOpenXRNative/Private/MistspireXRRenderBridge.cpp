#include "MistspireXRRenderBridge.h"
#include "MistspireOpenXRAccess.h"
#include "RenderingThread.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "DynamicRHI.h"
#include "Engine/Engine.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IOpenXRCore.h"
#include "Modules/ModuleManager.h"

static const uint64 XR_NULL_SYSTEM_ID = 0;

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
	return FMistspireOpenXRAccess::IsOpenXRAvailable();
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

	XrInstance Instance = nullptr;
	XrSession Session = nullptr;
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

	IHeadMountedDisplay* HMD = XRTracking->GetHMDDevice();
	if (!HMD)
	{
		UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: No HMD device"));
		return;
	}

	// Probe swapchain formats via OpenXR core
	IOpenXRCore* OpenXRCore = FModuleManager::GetModulePtr<IOpenXRCore>(TEXT("OpenXRCore"));
	if (OpenXRCore)
	{
		TArray<int64> Formats = OpenXRCore->EnumerateSwapchainFormats();
		if (Formats.Num() > 0)
		{
			PreferredSwapchainFormat = Formats[0];
			bNativeXrReady = true;
			UE_LOG(LogTemp, Log, TEXT("MistspireXRRenderBridge: Initialized — %d formats available, prefer=%lld, device=0x%p"),
				Formats.Num(), PreferredSwapchainFormat, NativeDevice);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MistspireXRRenderBridge: No swapchain formats"));
		}
	}

	// Log graphics adapter info
	const FString RHIName = GDynamicRHI ? GDynamicRHI->GetName() : TEXT("None");
	UE_LOG(LogTemp, Log, TEXT("MistspireXRRenderBridge: RHI=%s, Instance=0x%p, Session=0x%p"),
		*RHIName, Instance, Session);

	// Probe view configurations on the game thread
	FFunctionGraphTask::CreateAndDispatchWhenReady(
		[this, Instance, Session]()
		{
			EnqueueViewProbe(Instance, Session);
		},
		TStatId(), nullptr, ENamedThreads::GameThread);
}

void UMistspireXRRenderBridge::EnqueueViewProbe(void* InstanceHandle, void* SessionHandle)
{
	IOpenXRCore* OpenXRCore = FModuleManager::GetModulePtr<IOpenXRCore>(TEXT("OpenXRCore"));
	if (!OpenXRCore)
	{
		return;
	}

	// Get system ID from the session
	XrInstance Instance = static_cast<XrInstance>(InstanceHandle);
	XrSession Session = static_cast<XrSession>(SessionHandle);
	uint64 SystemId = GetXrSystemId(SessionHandle);

	UE_LOG(LogTemp, Verbose, TEXT("MistspireXRRenderBridge: View probe — SystemId=%llu"), SystemId);

	// Notify the tracking system that we're ready
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->OnBeginPlay();
	}
}

uint64 UMistspireXRRenderBridge::GetXrSystemId(void* SessionHandle) const
{
	IOpenXRCore* OpenXRCore = FModuleManager::GetModulePtr<IOpenXRCore>(TEXT("OpenXRCore"));
	if (!OpenXRCore)
	{
		return XR_NULL_SYSTEM_ID;
	}
	return OpenXRCore->GetSystemId();
}

void UMistspireXRRenderBridge::DestroySwapchain()
{
	bNativeXrReady = false;
	PreferredSwapchainFormat = 0;
}

const TArray<FXrGraphicsBinding>& UMistspireXRRenderBridge::GetOpenXrGraphicsBindings(void* InstanceHandle) const
{
	static TArray<FXrGraphicsBinding> CachedBindings;
	if (CachedBindings.IsEmpty())
	{
		IOpenXRCore* OpenXRCore = FModuleManager::GetModulePtr<IOpenXRCore>(TEXT("OpenXRCore"));
		if (OpenXRCore)
		{
			CachedBindings = OpenXRCore->EnumerateGraphicsBinding();
		}
	}
	return CachedBindings;
}
