#include "MistspireSettingsPanel.h"
#include "MistspireGameUserSettings.h"
#include "MistspireVRPawn.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/EngineVersion.h"

namespace
{
	const FSliderStyle& GetVisibleSliderStyle()
	{
		static const FSliderStyle Style = []()
		{
			const FSlateBrush* WhiteBox = FCoreStyle::Get().GetBrush("GenericWhiteBox");
			FSlateBrush Bar = WhiteBox ? *WhiteBox : FSlateBrush();
			Bar.DrawAs = ESlateBrushDrawType::Box;
			Bar.TintColor = FSlateColor(FLinearColor(0.82f, 0.84f, 0.88f, 1.f));
			Bar.ImageSize = FVector2D(32.f, 10.f);
			FSlateBrush Thumb = WhiteBox ? *WhiteBox : FSlateBrush();
			Thumb.DrawAs = ESlateBrushDrawType::Box;
			Thumb.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f));
			Thumb.ImageSize = FVector2D(18.f, 18.f);
			FSliderStyle Out;
			Out.SetNormalBarImage(Bar);
			Out.SetHoveredBarImage(Bar);
			Out.SetDisabledBarImage(Bar);
			Out.SetNormalThumbImage(Thumb);
			Out.SetHoveredThumbImage(Thumb);
			Out.SetDisabledThumbImage(Thumb);
			Out.SetBarThickness(10.f);
			return Out;
		}();
		return Style;
	}

	TSharedRef<SWidget> MakeSectionHeader(const FText& Label, const FSlateFontInfo& Font)
	{
		return SNew(STextBlock)
			.Text(Label)
			.Font(Font)
			.ColorAndOpacity(FLinearColor(0.75f, 0.82f, 0.95f, 1.f));
	}

	TSharedRef<SWidget> MakeCheckboxRow(
		const FText& Label,
		const FSlateFontInfo& Font,
		const TAttribute<ECheckBoxState>& IsChecked,
		FOnCheckStateChanged OnChanged)
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 12.f, 0.f)
			[
				SNew(SCheckBox).IsChecked(IsChecked).OnCheckStateChanged(OnChanged)
			]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(Font)
				.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
			];
	}

	TSharedRef<SWidget> MakeCycleRow(
		const FText& Label,
		const FSlateFontInfo& Font,
		const TAttribute<FText>& ValueText,
		FOnClicked OnClicked)
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(0.45f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(Label)
				.Font(Font)
				.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
			]
			+ SHorizontalBox::Slot().FillWidth(0.55f)
			[
				SNew(SButton).OnClicked(OnClicked)
				[
					SNew(STextBlock).Text(ValueText).Font(Font)
				]
			];
	}

	TSharedRef<SWidget> MakeSlider(
		TAttribute<float> Value,
		FOnFloatValueChanged OnChanged,
		float Min, float Max, float Step)
	{
		return SNew(SBox).MinDesiredHeight(22.f)
		[
			SNew(SSlider)
			.Style(&GetVisibleSliderStyle())
			.Value(Value)
			.OnValueChanged(OnChanged)
			.MinValue(Min)
			.MaxValue(Max)
			.StepSize(Step)
		];
	}
}

void SMistspireSettingsPanel::Construct(const FArguments& InArgs)
{
	OwnerPawn = InArgs._OwnerPawn;
	OnClosed = InArgs._OnClosed;

	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 26);
	const FSlateFontInfo SectionFont = FCoreStyle::GetDefaultFontStyle("Bold", 15);
	const FSlateFontInfo RowFont = FCoreStyle::GetDefaultFontStyle("Regular", 15);
	const FSlateFontInfo HintFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);

	TSharedRef<SVerticalBox> Rows = SNew(SVerticalBox);

	Rows->AddSlot().AutoHeight().Padding(0.f, 0.f, 0.f, 14.f)
	[
		SNew(STextBlock)
		.Text(NSLOCTEXT("Mistspire", "SettingsTitle", "Settings"))
		.Font(TitleFont)
		.ColorAndOpacity(FLinearColor::White)
	];

	// Display / Graphics
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f, 0.f, 8.f)
	[ MakeSectionHeader(NSLOCTEXT("Mistspire", "SettingsSecDisplay", "Display"), SectionFont) ];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeCycleRow(NSLOCTEXT("Mistspire", "SettingsFullscreen", "Fullscreen mode"), RowFont,
			TAttribute<FText>(this, &SMistspireSettingsPanel::GetFullscreenLabel),
			FOnClicked::CreateSP(this, &SMistspireSettingsPanel::OnCycleFullscreenClicked))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 6.f)
	[
		MakeCycleRow(NSLOCTEXT("Mistspire", "SettingsResolution", "Resolution"), RowFont,
			TAttribute<FText>(this, &SMistspireSettingsPanel::GetResolutionLabel),
			FOnClicked::CreateSP(this, &SMistspireSettingsPanel::OnCycleResolutionClicked))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 6.f)
	[
		MakeCycleRow(NSLOCTEXT("Mistspire", "SettingsQuality", "Graphics quality"), RowFont,
			TAttribute<FText>(this, &SMistspireSettingsPanel::GetQualityLabel),
			FOnClicked::CreateSP(this, &SMistspireSettingsPanel::OnCycleQualityClicked))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 6.f)
	[
		MakeCycleRow(NSLOCTEXT("Mistspire", "SettingsVSync", "VSync"), RowFont,
			TAttribute<FText>(this, &SMistspireSettingsPanel::GetVSyncLabel),
			FOnClicked::CreateSP(this, &SMistspireSettingsPanel::OnCycleVSyncClicked))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetFovLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetFovValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnFovChanged), 70.f, 110.f, 1.f)
	];

	// Controls
	Rows->AddSlot().AutoHeight().Padding(0.f, 16.f, 0.f, 8.f)
	[ MakeSectionHeader(NSLOCTEXT("Mistspire", "SettingsSecControls", "Controls"), SectionFont) ];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetSensitivityLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetSensitivityValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnSensitivityChanged), 0.25f, 3.f, 0.05f)
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsInvertY", "Invert Y look"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetInvertYState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnInvertYChanged))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 8.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsSprintToggle", "Sprint toggle (Shift)"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetSprintToggleState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnSprintToggleChanged))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 8.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsBob", "View bobbing"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetBobState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnBobChanged))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetBobScaleLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetBobScaleValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnBobScaleChanged), 0.f, 1.5f, 0.05f)
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsControls", "Show keyboard controls"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetControlsHintState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnControlsHintChanged))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 8.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsSnapTurn", "VR snap turn"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetSnapTurnState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnSnapTurnChanged))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetSnapDegreesLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetSnapDegreesValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnSnapDegreesChanged), 15.f, 90.f, 5.f)
	];

	// Audio
	Rows->AddSlot().AutoHeight().Padding(0.f, 16.f, 0.f, 8.f)
	[ MakeSectionHeader(NSLOCTEXT("Mistspire", "SettingsSecAudio", "Audio"), SectionFont) ];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetMasterVolumeLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetMasterVolumeValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnMasterVolumeChanged), 0.f, 1.f, 0.05f)
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetSfxVolumeLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetSfxVolumeValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnSfxVolumeChanged), 0.f, 1.f, 0.05f)
	];

	// Comfort / Accessibility
	Rows->AddSlot().AutoHeight().Padding(0.f, 16.f, 0.f, 8.f)
	[ MakeSectionHeader(NSLOCTEXT("Mistspire", "SettingsSecComfort", "Comfort & accessibility"), SectionFont) ];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetVignetteLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetVignetteValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnVignetteChanged), 0.f, 1.f, 0.05f)
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetMotionFovLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetMotionFovValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnMotionFovChanged), 0.f, 20.f, 1.f)
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(this, &SMistspireSettingsPanel::GetHudScaleLabel).Font(RowFont)
		.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeSlider(TAttribute<float>(this, &SMistspireSettingsPanel::GetHudScaleValue),
			FOnFloatValueChanged::CreateSP(this, &SMistspireSettingsPanel::OnHudScaleChanged), 0.75f, 1.75f, 0.05f)
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsHiContrast", "High-contrast crosshair"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetHighContrastState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnHighContrastChanged))
	];

	// Gameplay
	Rows->AddSlot().AutoHeight().Padding(0.f, 16.f, 0.f, 8.f)
	[ MakeSectionHeader(NSLOCTEXT("Mistspire", "SettingsSecGameplay", "Gameplay"), SectionFont) ];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsAltitudeHud", "Show altitude HUD"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetAltitudeHudState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnAltitudeHudChanged))
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 8.f)
	[
		MakeCheckboxRow(NSLOCTEXT("Mistspire", "SettingsAutoGlider", "Auto-glider on grapple release"), RowFont,
			TAttribute<ECheckBoxState>(this, &SMistspireSettingsPanel::GetAutoGliderState),
			FOnCheckStateChanged::CreateSP(this, &SMistspireSettingsPanel::OnAutoGliderChanged))
	];

	// About
	Rows->AddSlot().AutoHeight().Padding(0.f, 16.f, 0.f, 8.f)
	[ MakeSectionHeader(NSLOCTEXT("Mistspire", "SettingsSecAbout", "About"), SectionFont) ];
	Rows->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(STextBlock)
		.Text(this, &SMistspireSettingsPanel::GetAboutLabel)
		.Font(HintFont)
		.ColorAndOpacity(FLinearColor(0.7f, 0.72f, 0.76f))
		.AutoWrapText(true)
	];

	// Footer
	Rows->AddSlot().AutoHeight().Padding(0.f, 18.f, 0.f, 0.f)
	[
		SNew(SButton).HAlign(HAlign_Center)
		.OnClicked(this, &SMistspireSettingsPanel::OnResetDefaultsClicked)
		[
			SNew(STextBlock).Text(NSLOCTEXT("Mistspire", "SettingsReset", "Reset to defaults")).Font(RowFont)
		]
	];
	Rows->AddSlot().AutoHeight().Padding(0.f, 8.f, 0.f, 0.f)
	[
		SNew(SButton).HAlign(HAlign_Center)
		.OnClicked(this, &SMistspireSettingsPanel::OnResumeClicked)
		[
			SNew(STextBlock).Text(NSLOCTEXT("Mistspire", "SettingsResume", "Resume")).Font(RowFont)
		]
	];
	if (!GIsEditor)
	{
		Rows->AddSlot().AutoHeight().Padding(0.f, 8.f, 0.f, 0.f)
		[
			SNew(SButton).HAlign(HAlign_Center)
			.OnClicked(this, &SMistspireSettingsPanel::OnQuitClicked)
			[
				SNew(STextBlock).Text(NSLOCTEXT("Mistspire", "SettingsQuit", "Quit to desktop")).Font(RowFont)
			]
		];
	}
	Rows->AddSlot().AutoHeight().Padding(0.f, 10.f, 0.f, 0.f)
	[
		SNew(STextBlock)
		.Text(NSLOCTEXT("Mistspire", "SettingsEscHint", "Esc to close"))
		.Font(HintFont)
		.ColorAndOpacity(FLinearColor(0.65f, 0.65f, 0.65f))
	];

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(24.f))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.88f))
		[
			SNew(SBox)
			.WidthOverride(480.f)
			.MaxDesiredHeight(580.f)
			[
				SNew(SScrollBox)
				+ SScrollBox::Slot()
				[ Rows ]
			]
		]
	];
}

FReply SMistspireSettingsPanel::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		SaveSettings();
		OnClosed.ExecuteIfBound();
		return FReply::Handled();
	}
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SMistspireSettingsPanel::OnResumeClicked()
{
	SaveSettings();
	OnClosed.ExecuteIfBound();
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnQuitClicked()
{
	SaveSettings();
	if (AMistspireVRPawn* Pawn = OwnerPawn.Get())
	{
		if (UWorld* World = Pawn->GetWorld())
		{
			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
		}
	}
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnResetDefaultsClicked()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->SetToDefaults();
		Settings->ApplyResolutionSettings(false);
		Settings->ApplySettings(false);
		ApplyLive();
		SaveSettings();
	}
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnCycleFullscreenClicked()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->CycleFullscreenMode();
		SaveSettings();
	}
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnCycleResolutionClicked()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->CycleResolutionPreset();
		SaveSettings();
	}
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnCycleQualityClicked()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->CycleGraphicsQuality();
		SaveSettings();
	}
	return FReply::Handled();
}

FReply SMistspireSettingsPanel::OnCycleVSyncClicked()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->CycleVSync();
		SaveSettings();
	}
	return FReply::Handled();
}

#define MISTSPIRE_SETTINGS_FLOAT(Setter, Apply) \
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get()) { Settings->Setter(NewValue); if (Apply) ApplyLive(); }

void SMistspireSettingsPanel::OnSensitivityChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetMouseSensitivity, false) }
void SMistspireSettingsPanel::OnBobScaleChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetViewBobScale, false) }
void SMistspireSettingsPanel::OnFovChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetFieldOfView, true) }
void SMistspireSettingsPanel::OnMasterVolumeChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetMasterVolume, true) }
void SMistspireSettingsPanel::OnSfxVolumeChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetSfxVolume, true) }
void SMistspireSettingsPanel::OnVignetteChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetComfortVignetteStrength, false) }
void SMistspireSettingsPanel::OnMotionFovChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetMotionFovReduction, false) }
void SMistspireSettingsPanel::OnHudScaleChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetHudScale, false) }
void SMistspireSettingsPanel::OnSnapDegreesChanged(float NewValue) { MISTSPIRE_SETTINGS_FLOAT(SetSnapTurnDegrees, false) }

#undef MISTSPIRE_SETTINGS_FLOAT

void SMistspireSettingsPanel::OnBobChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetViewBobbingEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnInvertYChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetInvertYLookEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnControlsHintChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetControlsHintEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnAltitudeHudChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetAltitudeHudEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnAutoGliderChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetGrappleAutoGliderEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnSprintToggleChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetSprintToggleEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnSnapTurnChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetSnapTurnEnabled(NewState == ECheckBoxState::Checked);
}
void SMistspireSettingsPanel::OnHighContrastChanged(ECheckBoxState NewState)
{
	if (UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) S->SetHighContrastCrosshairEnabled(NewState == ECheckBoxState::Checked);
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

void SMistspireSettingsPanel::SaveSettings()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->SaveSettings();
	}
}

FText SMistspireSettingsPanel::GetFullscreenLabel() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetFullscreenModeLabel();
	return FText::GetEmpty();
}
FText SMistspireSettingsPanel::GetResolutionLabel() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetResolutionLabel();
	return FText::GetEmpty();
}
FText SMistspireSettingsPanel::GetQualityLabel() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetGraphicsQualityLabel();
	return FText::GetEmpty();
}
FText SMistspireSettingsPanel::GetVSyncLabel() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetVSyncLabel();
	return FText::GetEmpty();
}
FText SMistspireSettingsPanel::GetSensitivityLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsSensFmt", "Mouse sensitivity  {0}"),
		FText::AsNumber(FMath::RoundToFloat(GetSensitivityValue() * 100.f) / 100.f));
}
FText SMistspireSettingsPanel::GetBobScaleLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsBobScaleFmt", "View bob intensity  {0}"),
		FText::AsNumber(FMath::RoundToFloat(GetBobScaleValue() * 100.f) / 100.f));
}
FText SMistspireSettingsPanel::GetFovLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsFovFmt", "FOV  {0}"), FText::AsNumber(FMath::RoundToInt(GetFovValue())));
}
FText SMistspireSettingsPanel::GetMasterVolumeLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsMasterFmt", "Master volume  {0}%"),
		FText::AsNumber(FMath::RoundToInt(GetMasterVolumeValue() * 100.f)));
}
FText SMistspireSettingsPanel::GetSfxVolumeLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsSfxFmt", "SFX volume  {0}%"),
		FText::AsNumber(FMath::RoundToInt(GetSfxVolumeValue() * 100.f)));
}
FText SMistspireSettingsPanel::GetVignetteLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsVignetteFmt", "Comfort vignette  {0}%"),
		FText::AsNumber(FMath::RoundToInt(GetVignetteValue() * 100.f)));
}
FText SMistspireSettingsPanel::GetMotionFovLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsMotionFovFmt", "Motion FOV reduce  {0}°"),
		FText::AsNumber(FMath::RoundToInt(GetMotionFovValue())));
}
FText SMistspireSettingsPanel::GetHudScaleLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsHudScaleFmt", "HUD text scale  {0}"),
		FText::AsNumber(FMath::RoundToFloat(GetHudScaleValue() * 100.f) / 100.f));
}
FText SMistspireSettingsPanel::GetSnapDegreesLabel() const
{
	return FText::Format(NSLOCTEXT("Mistspire", "SettingsSnapDegFmt", "Snap turn angle  {0}°"),
		FText::AsNumber(FMath::RoundToInt(GetSnapDegreesValue())));
}
FText SMistspireSettingsPanel::GetAboutLabel() const
{
	return FText::FromString(FString::Printf(
		TEXT("Project Mistspire — Deepiri demo\nUnreal Engine %s"),
		*FEngineVersion::Current().ToString()));
}

float SMistspireSettingsPanel::GetSensitivityValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetMouseSensitivity();
	return 1.f;
}
float SMistspireSettingsPanel::GetBobScaleValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetViewBobScale();
	return 1.f;
}
float SMistspireSettingsPanel::GetFovValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetFieldOfView();
	return 90.f;
}
float SMistspireSettingsPanel::GetMasterVolumeValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetMasterVolume();
	return 1.f;
}
float SMistspireSettingsPanel::GetSfxVolumeValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetSfxVolume();
	return 1.f;
}
float SMistspireSettingsPanel::GetVignetteValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetComfortVignetteStrength();
	return 1.f;
}
float SMistspireSettingsPanel::GetMotionFovValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetMotionFovReduction();
	return 0.f;
}
float SMistspireSettingsPanel::GetHudScaleValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetHudScale();
	return 1.f;
}
float SMistspireSettingsPanel::GetSnapDegreesValue() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get()) return S->GetSnapTurnDegrees();
	return 30.f;
}

ECheckBoxState SMistspireSettingsPanel::GetBobState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsViewBobbingEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Checked;
}
ECheckBoxState SMistspireSettingsPanel::GetInvertYState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsInvertYLookEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Unchecked;
}
ECheckBoxState SMistspireSettingsPanel::GetControlsHintState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsControlsHintEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Unchecked;
}
ECheckBoxState SMistspireSettingsPanel::GetAltitudeHudState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsAltitudeHudEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Checked;
}
ECheckBoxState SMistspireSettingsPanel::GetAutoGliderState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsGrappleAutoGliderEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Checked;
}
ECheckBoxState SMistspireSettingsPanel::GetSprintToggleState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsSprintToggleEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Unchecked;
}
ECheckBoxState SMistspireSettingsPanel::GetSnapTurnState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsSnapTurnEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Unchecked;
}
ECheckBoxState SMistspireSettingsPanel::GetHighContrastState() const
{
	if (const UMistspireGameUserSettings* S = UMistspireGameUserSettings::Get())
		return S->IsHighContrastCrosshairEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	return ECheckBoxState::Unchecked;
}
