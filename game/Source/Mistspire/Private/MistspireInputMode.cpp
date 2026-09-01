#include "MistspireInputMode.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Misc/Parse.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/InputSettings.h"
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

#if WITH_EDITOR
	if (World && World->IsPlayInEditor())
	{
		// Plain Play = non-VR; VR Preview enables stereo rendering.
		return !IsStereoRenderingActive();
	}
#endif

	if (IsStereoRenderingActive())
	{
		return false;
	}

	return !UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
}

const TCHAR* FMistspireInputMode::GetNonVRControlsHint()
{
	return TEXT("WASD move | Mouse look | Space jump | LCtrl climb | Shift sprint | F grapple | G glider | T teleport | E interact");
}

void FMistspireInputMode::EnsureLegacyNonVRKeyMappings()
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();
	if (!Settings)
	{
		return;
	}

	auto AddAxis = [Settings](const FName& AxisName, const FKey& Key, float Scale)
	{
		Settings->AddAxisMapping(FInputAxisKeyMapping(AxisName, Key, Scale), false);
	};
	auto AddAction = [Settings](const FName& ActionName, const FKey& Key)
	{
		Settings->AddActionMapping(FInputActionKeyMapping(ActionName, Key), false);
	};

	AddAxis(TEXT("MoveForward"), EKeys::W, 1.f);
	AddAxis(TEXT("MoveForward"), EKeys::S, -1.f);
	AddAxis(TEXT("MoveRight"), EKeys::D, 1.f);
	AddAxis(TEXT("MoveRight"), EKeys::A, -1.f);
	AddAxis(TEXT("Turn"), EKeys::MouseX, 1.f);
	AddAxis(TEXT("LookUp"), EKeys::MouseY, -1.f);

	AddAction(TEXT("Jump"), EKeys::SpaceBar);
	AddAction(TEXT("Climb"), EKeys::LeftControl);
	AddAction(TEXT("Sprint"), EKeys::LeftShift);
	AddAction(TEXT("Grapple"), EKeys::F);
	AddAction(TEXT("Grapple"), EKeys::RightMouseButton);
	AddAction(TEXT("Glider"), EKeys::G);
	AddAction(TEXT("Teleport"), EKeys::T);
	AddAction(TEXT("Interact"), EKeys::E);

	Settings->ForceRebuildKeymaps();
}
