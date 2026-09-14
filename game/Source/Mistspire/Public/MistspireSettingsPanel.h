#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class AMistspireVRPawn;

DECLARE_DELEGATE(FOnMistspireSettingsClosed);

/** Non-VR Esc settings overlay (pure Slate, no Content widgets). */
class SMistspireSettingsPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMistspireSettingsPanel) {}
		SLATE_ARGUMENT(TWeakObjectPtr<AMistspireVRPawn>, OwnerPawn)
		SLATE_EVENT(FOnMistspireSettingsClosed, OnClosed)
	SLATE_END_ARGS()

	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<AMistspireVRPawn> OwnerPawn;
	FOnMistspireSettingsClosed OnClosed;

	FReply OnResumeClicked();
	FReply OnQuitClicked();
	FReply OnResetDefaultsClicked();
	FReply OnCycleFullscreenClicked();
	FReply OnCycleResolutionClicked();
	FReply OnCycleQualityClicked();
	FReply OnCycleVSyncClicked();

	void OnSensitivityChanged(float NewValue);
	void OnBobScaleChanged(float NewValue);
	void OnFovChanged(float NewValue);
	void OnMasterVolumeChanged(float NewValue);
	void OnSfxVolumeChanged(float NewValue);
	void OnVignetteChanged(float NewValue);
	void OnMotionFovChanged(float NewValue);
	void OnHudScaleChanged(float NewValue);
	void OnSnapDegreesChanged(float NewValue);

	void OnBobChanged(ECheckBoxState NewState);
	void OnInvertYChanged(ECheckBoxState NewState);
	void OnControlsHintChanged(ECheckBoxState NewState);
	void OnAltitudeHudChanged(ECheckBoxState NewState);
	void OnAutoGliderChanged(ECheckBoxState NewState);
	void OnSprintToggleChanged(ECheckBoxState NewState);
	void OnSnapTurnChanged(ECheckBoxState NewState);
	void OnHighContrastChanged(ECheckBoxState NewState);

	FText GetFullscreenLabel() const;
	FText GetResolutionLabel() const;
	FText GetQualityLabel() const;
	FText GetVSyncLabel() const;
	FText GetSensitivityLabel() const;
	FText GetBobScaleLabel() const;
	FText GetFovLabel() const;
	FText GetMasterVolumeLabel() const;
	FText GetSfxVolumeLabel() const;
	FText GetVignetteLabel() const;
	FText GetMotionFovLabel() const;
	FText GetHudScaleLabel() const;
	FText GetSnapDegreesLabel() const;
	FText GetAboutLabel() const;

	float GetSensitivityValue() const;
	float GetBobScaleValue() const;
	float GetFovValue() const;
	float GetMasterVolumeValue() const;
	float GetSfxVolumeValue() const;
	float GetVignetteValue() const;
	float GetMotionFovValue() const;
	float GetHudScaleValue() const;
	float GetSnapDegreesValue() const;

	ECheckBoxState GetBobState() const;
	ECheckBoxState GetInvertYState() const;
	ECheckBoxState GetControlsHintState() const;
	ECheckBoxState GetAltitudeHudState() const;
	ECheckBoxState GetAutoGliderState() const;
	ECheckBoxState GetSprintToggleState() const;
	ECheckBoxState GetSnapTurnState() const;
	ECheckBoxState GetHighContrastState() const;

	void ApplyLive();
	void SaveSettings();
};
