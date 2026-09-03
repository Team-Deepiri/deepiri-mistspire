#include "MistspireInputMode.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "IXRTrackingSystem.h"
#include "Misc/Parse.h"
#include "HAL/IConsoleManager.h"
#include "StereoRendering.h"

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
			const TSharedPtr<IStereoRendering, ESPMode::ThreadSafe> Stereo = GEngine->XRSystem->GetStereoRenderingDevice();
			return Stereo.IsValid() && Stereo->IsStereoEnabled();
		}
		return false;
	}

	UInputAction* NewAction(UObject* Outer, FName Name, EInputActionValueType ValueType)
	{
		UInputAction* Action = NewObject<UInputAction>(Outer, Name);
		Action->ValueType = ValueType;
		if (ValueType == EInputActionValueType::Axis2D)
		{
			Action->AccumulationBehavior = EInputActionAccumulationBehavior::Cumulative;
		}
		return Action;
	}

	template <typename ModifierType>
	ModifierType* AddModifier(UInputMappingContext* Context, FEnhancedActionKeyMapping& Mapping)
	{
		ModifierType* Modifier = NewObject<ModifierType>(Context);
		Mapping.Modifiers.Add(Modifier);
		return Modifier;
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

void FMistspireInputMode::CreateNonVREnhancedInput(UObject* Outer, FMistspireNonVREnhancedInput& Out)
{
	if (!Outer)
	{
		return;
	}

	Out.MappingContext = NewObject<UInputMappingContext>(Outer, TEXT("IMC_MistspireNonVR"));
	UInputMappingContext* Context = Out.MappingContext;

	Out.Move = NewAction(Context, TEXT("IA_NonVR_Move"), EInputActionValueType::Axis2D);
	Out.Look = NewAction(Context, TEXT("IA_NonVR_Look"), EInputActionValueType::Axis2D);
	Out.Jump = NewAction(Context, TEXT("IA_NonVR_Jump"), EInputActionValueType::Boolean);
	Out.Climb = NewAction(Context, TEXT("IA_NonVR_Climb"), EInputActionValueType::Boolean);
	Out.Sprint = NewAction(Context, TEXT("IA_NonVR_Sprint"), EInputActionValueType::Boolean);
	Out.Grapple = NewAction(Context, TEXT("IA_NonVR_Grapple"), EInputActionValueType::Boolean);
	Out.Glider = NewAction(Context, TEXT("IA_NonVR_Glider"), EInputActionValueType::Boolean);
	Out.Teleport = NewAction(Context, TEXT("IA_NonVR_Teleport"), EInputActionValueType::Boolean);
	Out.Interact = NewAction(Context, TEXT("IA_NonVR_Interact"), EInputActionValueType::Boolean);

	Context->MapKey(Out.Move, EKeys::D);
	AddModifier<UInputModifierNegate>(Context, Context->MapKey(Out.Move, EKeys::A));
	AddModifier<UInputModifierSwizzleAxis>(Context, Context->MapKey(Out.Move, EKeys::W));
	{
		FEnhancedActionKeyMapping& South = Context->MapKey(Out.Move, EKeys::S);
		AddModifier<UInputModifierSwizzleAxis>(Context, South);
		AddModifier<UInputModifierNegate>(Context, South);
	}

	{
		FEnhancedActionKeyMapping& Mouse = Context->MapKey(Out.Look, EKeys::Mouse2D);
		UInputModifierScalar* Scale = AddModifier<UInputModifierScalar>(Context, Mouse);
		Scale->Scalar = FVector(0.14f, 0.14f, 1.f);
		UInputModifierNegate* InvertY = AddModifier<UInputModifierNegate>(Context, Mouse);
		InvertY->bX = false;
		InvertY->bY = true;
		InvertY->bZ = false;
	}

	Context->MapKey(Out.Jump, EKeys::SpaceBar);
	Context->MapKey(Out.Climb, EKeys::LeftControl);
	Context->MapKey(Out.Sprint, EKeys::LeftShift);
	Context->MapKey(Out.Grapple, EKeys::F);
	Context->MapKey(Out.Grapple, EKeys::RightMouseButton);
	Context->MapKey(Out.Glider, EKeys::G);
	Context->MapKey(Out.Teleport, EKeys::T);
	Context->MapKey(Out.Interact, EKeys::E);
}

void FMistspireInputMode::AddNonVRMappingContext(APlayerController* PlayerController, const UInputMappingContext* MappingContext)
{
	if (!PlayerController || !MappingContext)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->AddMappingContext(MappingContext, 0);
	}
}

void FMistspireInputMode::ApplyRendererOverrides(bool bNonVR)
{
	// Must use SetByCode: DefaultEngine.ini RendererSettings bind as SetByProjectSetting,
	// which silently wins over SetByGameSetting — prior overrides never applied.
	auto SetCVar = [](const TCHAR* Name, int32 Value)
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			CVar->Set(Value, ECVF_SetByCode);
		}
	};

	if (bNonVR)
	{
		// Two bottom stereo tiles + right-edge strip = leftover L/R eye rects in a mono view.
		SetCVar(TEXT("r.ClearSceneMethod"), 1);
		SetCVar(TEXT("vr.InstancedStereo"), 0);
		SetCVar(TEXT("vr.StereoViewOffset"), 0);
		SetCVar(TEXT("r.VariableRateShading.Enable"), 0);
		SetCVar(TEXT("r.VolumetricFog.ScreenResolutionDivisor"), 1);
		// Fullscreen / windowed is owned by UMistspireGameUserSettings (Esc menu + defaults).
	}
	else
	{
		// VR HMD writes every pixel; skip clear + enable stereo packing for perf.
		SetCVar(TEXT("r.ClearSceneMethod"), 0);
		SetCVar(TEXT("vr.InstancedStereo"), 1);
		SetCVar(TEXT("vr.StereoViewOffset"), 1);
		SetCVar(TEXT("r.VariableRateShading.Enable"), 1);
		SetCVar(TEXT("r.VolumetricFog.ScreenResolutionDivisor"), 2);
	}
}
