#include "MistspireGameUserSettings.h"
#include "MistspireAudioSubsystem.h"
#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireVRPawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GenericPlatform/GenericWindow.h"
#include "Scalability.h"

namespace
{
	struct FMistspireResPreset
	{
		int32 X;
		int32 Y;
		const TCHAR* Label;
	};

	const FMistspireResPreset ResPresets[] = {
		{1280, 720, TEXT("1280×720")},
		{1600, 900, TEXT("1600×900")},
		{1920, 1080, TEXT("1920×1080")},
		{2560, 1440, TEXT("2560×1440")},
		{3840, 2160, TEXT("3840×2160")},
	};
	constexpr int32 ResPresetCount = UE_ARRAY_COUNT(ResPresets);
}

UMistspireGameUserSettings::UMistspireGameUserSettings()
{
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

void UMistspireGameUserSettings::SetInvertYLookEnabled(bool bEnabled) { bInvertYLook = bEnabled; }
void UMistspireGameUserSettings::SetViewBobbingEnabled(bool bEnabled) { bViewBobbing = bEnabled; }

void UMistspireGameUserSettings::SetViewBobScale(float Value)
{
	ViewBobScale = FMath::Clamp(Value, 0.f, 1.5f);
}

void UMistspireGameUserSettings::SetFieldOfView(float Degrees)
{
	FieldOfView = FMath::Clamp(Degrees, 70.f, 110.f);
}

void UMistspireGameUserSettings::SetControlsHintEnabled(bool bEnabled) { bShowControlsHint = bEnabled; }

void UMistspireGameUserSettings::SetAltitudeHudEnabled(bool bEnabled)
{
	bShowAltitudeHud = bEnabled;
	UMistspireAltitudeDebugSubsystem::SetHudEnabled(bEnabled);
}

void UMistspireGameUserSettings::SetMasterVolume(float Value)
{
	MasterVolume = FMath::Clamp(Value, 0.f, 1.f);
}

void UMistspireGameUserSettings::SetSfxVolume(float Value)
{
	SfxVolume = FMath::Clamp(Value, 0.f, 1.f);
}

void UMistspireGameUserSettings::SetGrappleAutoGliderEnabled(bool bEnabled) { bGrappleAutoGlider = bEnabled; }
void UMistspireGameUserSettings::SetSprintToggleEnabled(bool bEnabled) { bSprintToggle = bEnabled; }

void UMistspireGameUserSettings::SetComfortVignetteStrength(float Value)
{
	ComfortVignetteStrength = FMath::Clamp(Value, 0.f, 1.f);
}

void UMistspireGameUserSettings::SetSnapTurnEnabled(bool bEnabled) { bSnapTurn = bEnabled; }

void UMistspireGameUserSettings::SetSnapTurnDegrees(float Value)
{
	SnapTurnDegrees = FMath::Clamp(Value, 15.f, 90.f);
}

void UMistspireGameUserSettings::SetMotionFovReduction(float Value)
{
	MotionFovReduction = FMath::Clamp(Value, 0.f, 20.f);
}

void UMistspireGameUserSettings::SetHudScale(float Value)
{
	HudScale = FMath::Clamp(Value, 0.75f, 1.75f);
}

void UMistspireGameUserSettings::SetHighContrastCrosshairEnabled(bool bEnabled)
{
	bHighContrastCrosshair = bEnabled;
}

void UMistspireGameUserSettings::SetGraphicsQualityPreset(int32 Preset)
{
	GraphicsQualityPreset = FMath::Clamp(Preset, 0, 3);
	SetOverallScalabilityLevel(GraphicsQualityPreset);
	ApplySettings(false);
}

void UMistspireGameUserSettings::CycleGraphicsQuality()
{
	SetGraphicsQualityPreset((GraphicsQualityPreset + 1) % 4);
}

FText UMistspireGameUserSettings::GetGraphicsQualityLabel() const
{
	switch (GraphicsQualityPreset)
	{
	case 0: return NSLOCTEXT("Mistspire", "GQ_Low", "Low");
	case 1: return NSLOCTEXT("Mistspire", "GQ_Medium", "Medium");
	case 2: return NSLOCTEXT("Mistspire", "GQ_High", "High");
	default: return NSLOCTEXT("Mistspire", "GQ_Epic", "Epic");
	}
}

void UMistspireGameUserSettings::CycleResolutionPreset()
{
	ResolutionPresetIndex = (ResolutionPresetIndex + 1) % ResPresetCount;
	const FMistspireResPreset& Preset = ResPresets[ResolutionPresetIndex];
	SetScreenResolution(FIntPoint(Preset.X, Preset.Y));
	ApplyResolutionSettings(false);
	ApplySettings(false);
}

FText UMistspireGameUserSettings::GetResolutionLabel() const
{
	const int32 Idx = FMath::Clamp(ResolutionPresetIndex, 0, ResPresetCount - 1);
	return FText::FromString(ResPresets[Idx].Label);
}

void UMistspireGameUserSettings::ApplyAudioVolumes(UWorld* World) const
{
	if (!World)
	{
		return;
	}

	if (UMistspireAudioSubsystem* Audio = World->GetSubsystem<UMistspireAudioSubsystem>())
	{
		Audio->SetChannelVolume(EMistspireAudioChannel::Ambient, MasterVolume);
		Audio->SetChannelVolume(EMistspireAudioChannel::Weather, MasterVolume);
		Audio->SetChannelVolume(EMistspireAudioChannel::UI, SfxVolume);
		Audio->SetChannelVolume(EMistspireAudioChannel::Spatial, SfxVolume);
		Audio->SetChannelVolume(EMistspireAudioChannel::Surface, SfxVolume);
		Audio->SetChannelVolume(EMistspireAudioChannel::Physiology, SfxVolume);
	}
}

void UMistspireGameUserSettings::ApplyGameplaySettings(AMistspireVRPawn* Pawn)
{
	if (!Pawn)
	{
		return;
	}

	if (Pawn->IsNonVRMode() && Pawn->VRCamera)
	{
		Pawn->VRCamera->SetFieldOfView(FieldOfView);
	}

	UMistspireAltitudeDebugSubsystem::SetHudEnabled(bShowAltitudeHud);
	ApplyAudioVolumes(Pawn->GetWorld());
	SetOverallScalabilityLevel(GraphicsQualityPreset);
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

void UMistspireGameUserSettings::CycleVSync()
{
	SetVSyncEnabled(!IsVSyncEnabled());
	ApplySettings(false);
}

FText UMistspireGameUserSettings::GetVSyncLabel() const
{
	return IsVSyncEnabled()
		? NSLOCTEXT("Mistspire", "VSync_On", "On")
		: NSLOCTEXT("Mistspire", "VSync_Off", "Off");
}

void UMistspireGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	MouseSensitivity = 1.f;
	bInvertYLook = false;
	bViewBobbing = true;
	ViewBobScale = 1.f;
	FieldOfView = 90.f;
	bShowControlsHint = false;
	bShowAltitudeHud = true;
	MasterVolume = 1.f;
	SfxVolume = 1.f;
	bGrappleAutoGlider = true;
	bSprintToggle = false;
	ComfortVignetteStrength = 1.f;
	bSnapTurn = false;
	SnapTurnDegrees = 30.f;
	MotionFovReduction = 0.f;
	HudScale = 1.f;
	bHighContrastCrosshair = false;
	GraphicsQualityPreset = 2;
	ResolutionPresetIndex = 2;
	SetFullscreenMode(EWindowMode::Windowed);
	SetVSyncEnabled(false);
	SetScreenResolution(FIntPoint(1920, 1080));
	SetOverallScalabilityLevel(GraphicsQualityPreset);
	UMistspireAltitudeDebugSubsystem::SetHudEnabled(true);
}
