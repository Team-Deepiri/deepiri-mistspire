#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "MistspireGameUserSettings.generated.h"

class AMistspireVRPawn;
class UWorld;

/** Persisted non-VR display / feel settings (Esc menu). */
UCLASS(config = GameUserSettings, configdonotcheckdefaults)
class MISTSPIRE_API UMistspireGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UMistspireGameUserSettings();

	static UMistspireGameUserSettings* Get();

	float GetMouseSensitivity() const { return MouseSensitivity; }
	void SetMouseSensitivity(float Value);

	bool IsInvertYLookEnabled() const { return bInvertYLook; }
	void SetInvertYLookEnabled(bool bEnabled);

	bool IsViewBobbingEnabled() const { return bViewBobbing; }
	void SetViewBobbingEnabled(bool bEnabled);

	float GetViewBobScale() const { return ViewBobScale; }
	void SetViewBobScale(float Value);

	float GetFieldOfView() const { return FieldOfView; }
	void SetFieldOfView(float Degrees);

	bool IsControlsHintEnabled() const { return bShowControlsHint; }
	void SetControlsHintEnabled(bool bEnabled);

	bool IsAltitudeHudEnabled() const { return bShowAltitudeHud; }
	void SetAltitudeHudEnabled(bool bEnabled);

	float GetMasterVolume() const { return MasterVolume; }
	void SetMasterVolume(float Value);

	float GetSfxVolume() const { return SfxVolume; }
	void SetSfxVolume(float Value);

	bool IsGrappleAutoGliderEnabled() const { return bGrappleAutoGlider; }
	void SetGrappleAutoGliderEnabled(bool bEnabled);

	bool IsSprintToggleEnabled() const { return bSprintToggle; }
	void SetSprintToggleEnabled(bool bEnabled);

	float GetComfortVignetteStrength() const { return ComfortVignetteStrength; }
	void SetComfortVignetteStrength(float Value);

	bool IsSnapTurnEnabled() const { return bSnapTurn; }
	void SetSnapTurnEnabled(bool bEnabled);

	float GetSnapTurnDegrees() const { return SnapTurnDegrees; }
	void SetSnapTurnDegrees(float Value);

	float GetMotionFovReduction() const { return MotionFovReduction; }
	void SetMotionFovReduction(float Value);

	float GetHudScale() const { return HudScale; }
	void SetHudScale(float Value);

	bool IsHighContrastCrosshairEnabled() const { return bHighContrastCrosshair; }
	void SetHighContrastCrosshairEnabled(bool bEnabled);

	int32 GetGraphicsQualityPreset() const { return GraphicsQualityPreset; }
	void SetGraphicsQualityPreset(int32 Preset);
	FText GetGraphicsQualityLabel() const;
	void CycleGraphicsQuality();

	void CycleResolutionPreset();
	FText GetResolutionLabel() const;

	void CycleFullscreenMode();
	FText GetFullscreenModeLabel() const;

	void CycleVSync();
	FText GetVSyncLabel() const;

	/** Apply FOV / audio / HUD prefs to the local pawn and world. */
	void ApplyGameplaySettings(AMistspireVRPawn* Pawn);

	/** Push master/SFX volumes onto Mistspire audio buses. */
	void ApplyAudioVolumes(UWorld* World) const;

	virtual void SetToDefaults() override;

protected:
	UPROPERTY(config)
	float MouseSensitivity = 1.f;

	UPROPERTY(config)
	bool bInvertYLook = false;

	UPROPERTY(config)
	bool bViewBobbing = true;

	UPROPERTY(config)
	float ViewBobScale = 1.f;

	UPROPERTY(config)
	float FieldOfView = 90.f;

	UPROPERTY(config)
	bool bShowControlsHint = false;

	UPROPERTY(config)
	bool bShowAltitudeHud = true;

	UPROPERTY(config)
	float MasterVolume = 1.f;

	UPROPERTY(config)
	float SfxVolume = 1.f;

	UPROPERTY(config)
	bool bGrappleAutoGlider = true;

	/** Shift toggles sprint instead of hold-to-sprint. */
	UPROPERTY(config)
	bool bSprintToggle = false;

	/** 0 = off, 1 = full comfort vignette (VR). */
	UPROPERTY(config)
	float ComfortVignetteStrength = 1.f;

	/** VR stick turn snaps in discrete steps when true. */
	UPROPERTY(config)
	bool bSnapTurn = false;

	UPROPERTY(config)
	float SnapTurnDegrees = 30.f;

	/** Degrees subtracted from FOV while moving (non-VR comfort). */
	UPROPERTY(config)
	float MotionFovReduction = 0.f;

	/** Multiplier for on-screen altitude HUD text size. */
	UPROPERTY(config)
	float HudScale = 1.f;

	UPROPERTY(config)
	bool bHighContrastCrosshair = false;

	/** 0=Low 1=Medium 2=High 3=Epic */
	UPROPERTY(config)
	int32 GraphicsQualityPreset = 2;

	/** Index into resolution preset table (-1 = use current desktop). */
	UPROPERTY(config)
	int32 ResolutionPresetIndex = 2;
};
