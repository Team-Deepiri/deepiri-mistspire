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
	FReply OnCycleFullscreenClicked();
	void OnSensitivityChanged(float NewValue);
	void OnBobChanged(ECheckBoxState NewState);
	void OnControlsHintChanged(ECheckBoxState NewState);
	void OnFovChanged(float NewValue);

	FText GetFullscreenLabel() const;
	FText GetSensitivityLabel() const;
	FText GetFovLabel() const;
	float GetSensitivityValue() const;
	float GetFovValue() const;
	ECheckBoxState GetBobState() const;
	ECheckBoxState GetControlsHintState() const;

	void ApplyLive();
};
