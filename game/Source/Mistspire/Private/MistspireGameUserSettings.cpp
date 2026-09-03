#include "MistspireGameUserSettings.h"
#include "MistspireVRPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GenericPlatform/GenericWindow.h"

UMistspireGameUserSettings::UMistspireGameUserSettings()
{
	// Match DefaultGameUserSettings.ini windowed default for first launch.
	SetFullscreenMode(EWindowMode::Windowed);
	SetFrameRateLimit(0.f);
}

UMistspireGameUserSettings* UMistspireGameUserSettings::Get()
{
	return Cast<UMistspireGameUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr);
}

void UMistspireGameUserSettings::SetMouseSensitivity(float Value)
{
	MouseSensitivity = FMath::Clamp(Value, 0.25f, 3.f);
}

void UMistspireGameUserSettings::SetViewBobbingEnabled(bool bEnabled)
{
	bViewBobbing = bEnabled;
}

void UMistspireGameUserSettings::SetViewBobScale(float Value)
{
	ViewBobScale = FMath::Clamp(Value, 0.f, 1.f);
}

void UMistspireGameUserSettings::SetFieldOfView(float Degrees)
{
	FieldOfView = FMath::Clamp(Degrees, 70.f, 110.f);
}

void UMistspireGameUserSettings::SetControlsHintEnabled(bool bEnabled)
{
	bShowControlsHint = bEnabled;
}

void UMistspireGameUserSettings::ApplyGameplaySettings(AMistspireVRPawn* Pawn)
{
	if (!Pawn || !Pawn->IsNonVRMode())
	{
		return;
	}

	if (Pawn->VRCamera)
	{
		Pawn->VRCamera->SetFieldOfView(FieldOfView);
	}
}

void UMistspireGameUserSettings::CycleFullscreenMode()
{
	const EWindowMode::Type Current = GetFullscreenMode();
	EWindowMode::Type Next = EWindowMode::Windowed;
	switch (Current)
	{
	case EWindowMode::Windowed:
		Next = EWindowMode::WindowedFullscreen;
		break;
	case EWindowMode::WindowedFullscreen:
		Next = EWindowMode::Fullscreen;
		break;
	default:
		Next = EWindowMode::Windowed;
		break;
	}

	SetFullscreenMode(Next);
	FIntPoint Res = GetScreenResolution();
	if (Res.X < 640 || Res.Y < 480)
	{
		SetScreenResolution(FIntPoint(1920, 1080));
	}
	ApplyResolutionSettings(false);
	ApplySettings(false);
}

FText UMistspireGameUserSettings::GetFullscreenModeLabel() const
{
	switch (GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		return NSLOCTEXT("Mistspire", "FS_Fullscreen", "Fullscreen");
	case EWindowMode::WindowedFullscreen:
		return NSLOCTEXT("Mistspire", "FS_Borderless", "Borderless");
	default:
		return NSLOCTEXT("Mistspire", "FS_Windowed", "Windowed");
	}
}
