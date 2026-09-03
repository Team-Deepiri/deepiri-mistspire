#include "MistspireSettingsPanel.h"
#include "MistspireGameUserSettings.h"
#include "MistspireVRPawn.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"

void SMistspireSettingsPanel::Construct(const FArguments& InArgs)
{
	OwnerPawn = InArgs._OwnerPawn;
	OnClosed = InArgs._OnClosed;

	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 28);
	const FSlateFontInfo RowFont = FCoreStyle::GetDefaultFontStyle("Regular", 16);

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(28.f))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.82f))
		[
			SNew(SBox)
			.WidthOverride(420.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 18.f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("Mistspire", "SettingsTitle", "Settings"))
					.Font(TitleFont)
					.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(0.45f).VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Mistspire", "SettingsFullscreen", "Fullscreen mode"))
						.Font(RowFont)
						.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
					]
					+ SHorizontalBox::Slot().FillWidth(0.55f)
					[
						SNew(SButton)
						.OnClicked(this, &SMistspireSettingsPanel::OnCycleFullscreenClicked)
						[
							SNew(STextBlock)
							.Text(this, &SMistspireSettingsPanel::GetFullscreenLabel)
							.Font(RowFont)
						]
					]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f)
				[
					SNew(STextBlock)
					.Text(this, &SMistspireSettingsPanel::GetSensitivityLabel)
					.Font(RowFont)
					.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
				[
					SNew(SSlider)
					.Value(this, &SMistspireSettingsPanel::GetSensitivityValue)
					.OnValueChanged(this, &SMistspireSettingsPanel::OnSensitivityChanged)
					.MinValue(0.25f)
					.MaxValue(3.f)
					.StepSize(0.05f)
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 12.f, 0.f)
					[
						SNew(SCheckBox)
						.IsChecked(this, &SMistspireSettingsPanel::GetBobState)
						.OnCheckStateChanged(this, &SMistspireSettingsPanel::OnBobChanged)
					]
					+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Mistspire", "SettingsBob", "View bobbing"))
						.Font(RowFont)
						.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
					]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 12.f, 0.f)
					[
						SNew(SCheckBox)
						.IsChecked(this, &SMistspireSettingsPanel::GetControlsHintState)
						.OnCheckStateChanged(this, &SMistspireSettingsPanel::OnControlsHintChanged)
					]
					+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Mistspire", "SettingsControls", "Show keyboard controls"))
						.Font(RowFont)
						.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
					]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f)
				[
					SNew(STextBlock)
					.Text(this, &SMistspireSettingsPanel::GetFovLabel)
					.Font(RowFont)
					.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
				[
					SNew(SSlider)
					.Value(this, &SMistspireSettingsPanel::GetFovValue)
					.OnValueChanged(this, &SMistspireSettingsPanel::OnFovChanged)
					.MinValue(70.f)
					.MaxValue(110.f)
					.StepSize(1.f)
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 22.f, 0.f, 0.f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.OnClicked(this, &SMistspireSettingsPanel::OnResumeClicked)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Mistspire", "SettingsResume", "Resume"))
						.Font(RowFont)
					]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 8.f, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("Mistspire", "SettingsEscHint", "Esc to close"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
					.ColorAndOpacity(FLinearColor(0.65f, 0.65f, 0.65f))
				]
			]
		]
	];
}

FReply SMistspireSettingsPanel::OnResumeClicked()
{
	OnClosed.ExecuteIfBound();
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnCycleFullscreenClicked()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->CycleFullscreenMode();
	}
	return FReply::Handled();
}

void SMistspireSettingsPanel::OnSensitivityChanged(float NewValue)
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->SetMouseSensitivity(NewValue);
		ApplyLive();
	}
}

void SMistspireSettingsPanel::OnBobChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->SetViewBobbingEnabled(NewState == ECheckBoxState::Checked);
		ApplyLive();
	}
}

void SMistspireSettingsPanel::OnControlsHintChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->SetControlsHintEnabled(NewState == ECheckBoxState::Checked);
	}
}

void SMistspireSettingsPanel::OnFovChanged(float NewValue)
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->SetFieldOfView(NewValue);
		ApplyLive();
	}
}

void SMistspireSettingsPanel::ApplyLive()
{
	if (AMistspireVRPawn* Pawn = OwnerPawn.Get())
	{
		if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
		{
			Settings->ApplyGameplaySettings(Pawn);
		}
	}
}

FText SMistspireSettingsPanel::GetFullscreenLabel() const
{
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		return Settings->GetFullscreenModeLabel();
	}
	return FText::GetEmpty();
}

FText SMistspireSettingsPanel::GetSensitivityLabel() const
{
	const float Value = GetSensitivityValue();
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsSensFmt", "Mouse sensitivity  {0}"),
		FText::AsNumber(FMath::RoundToFloat(Value * 100.f) / 100.f));
}

FText SMistspireSettingsPanel::GetFovLabel() const
{
	const float Value = GetFovValue();
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsFovFmt", "FOV  {0}"),
		FText::AsNumber(FMath::RoundToInt(Value)));
}

float SMistspireSettingsPanel::GetSensitivityValue() const
{
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		return Settings->GetMouseSensitivity();
	}
	return 1.f;
}

float SMistspireSettingsPanel::GetFovValue() const
{
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		return Settings->GetFieldOfView();
	}
	return 90.f;
}

ECheckBoxState SMistspireSettingsPanel::GetBobState() const
{
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		return Settings->IsViewBobbingEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Checked;
}

ECheckBoxState SMistspireSettingsPanel::GetControlsHintState() const
{
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		return Settings->IsControlsHintEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}
