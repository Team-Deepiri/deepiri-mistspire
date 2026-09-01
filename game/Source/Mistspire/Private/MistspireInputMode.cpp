#include "MistspireInputMode.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Misc/Parse.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "IXRTrackingSystem.h"

namespace
{
	bool HasForceVRFlag()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("forcvr"))
			|| FParse::Param(FCommandLine::Get(), TEXT("forcevr"));
	}

	bool HasForceNonVRFlag()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("nonvr"))
			|| FParse::Param(FCommandLine::Get(), TEXT("demoflat"));
	}

	bool IsStereoRenderingActive()
	{
		if (GEngine && GEngine->IsStereoscopic3D())
		{
			return true;
		}
		if (GEngine && GEngine->XRSystem.IsValid())
		{
			return GEngine->XRSystem->IsStereoEnabled();
		}
		return false;
	}
}

bool FMistspireInputMode::IsNonVRMode(const UWorld* World)
{
	if (HasForceVRFlag())
	{
		return false;
	}

	if (HasForceNonVRFlag())
	{
		return true;
	}

	if (IsStereoRenderingActive())
	{
		return false;
	}

#if WITH_EDITOR
	if (World && World->IsPlayInEditor())
	{
		// Default editor Play is non-VR; VR Preview enables stereo via IsHeadMountedDisplayEnabled above.
		return true;
	}
#endif

	return !UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
}

const TCHAR* FMistspireInputMode::GetNonVRControlsHint()
{
	return TEXT("WASD move | Mouse look | Space jump | LCtrl climb | Shift sprint | F grapple | G glider | T teleport | E interact");
}
