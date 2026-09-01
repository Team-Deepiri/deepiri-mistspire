#include "MistspireInputMode.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Misc/Parse.h"
#include "Engine/World.h"

bool FMistspireInputMode::IsNonVRMode(const UWorld* World)
{
	if (FParse::Param(FCommandLine::Get(), TEXT("forcvr")))
	{
		return false;
	}

	if (FParse::Param(FCommandLine::Get(), TEXT("nonvr"))
		|| FParse::Param(FCommandLine::Get(), TEXT("demoflat")))
	{
		return true;
	}

#if WITH_EDITOR
	if (World && World->IsPlayInEditor())
	{
		if (FParse::Param(FCommandLine::Get(), TEXT("vr")) || FParse::Param(FCommandLine::Get(), TEXT("HMD")))
		{
			return false;
		}
		return true;
	}
#endif

	return !UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
}
