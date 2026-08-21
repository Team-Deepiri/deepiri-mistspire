#include "MistspireOpenXRAccess.h"
#include "IOpenXRHMD.h"
#include "IOpenXRHMDModule.h"
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"

bool FMistspireOpenXRAccess::IsOpenXRAvailable()
{
	if (!IOpenXRHMDModule::IsAvailable())
	{
		return false;
	}

	if (GEngine && GEngine->XRSystem.IsValid())
	{
		if (IOpenXRHMD* HMD = GEngine->XRSystem->GetIOpenXRHMD())
		{
			return HMD->IsInitialized();
		}
	}

	return IOpenXRHMDModule::Get().GetInstance() != XR_NULL_HANDLE;
}

bool FMistspireOpenXRAccess::GetNativeHandles(XrInstance& OutInstance, XrSession& OutSession)
{
	OutInstance = XR_NULL_HANDLE;
	OutSession = XR_NULL_HANDLE;

	if (!GEngine || !GEngine->XRSystem.IsValid())
	{
		return false;
	}

	IOpenXRHMD* HMD = GEngine->XRSystem->GetIOpenXRHMD();
	if (!HMD || !HMD->IsInitialized())
	{
		return false;
	}

	OutInstance = HMD->GetInstance();
	OutSession = HMD->GetSession();
	return OutInstance != XR_NULL_HANDLE && OutSession != XR_NULL_HANDLE;
}
