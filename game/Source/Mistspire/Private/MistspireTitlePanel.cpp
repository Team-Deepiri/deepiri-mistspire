#include "MistspireTitlePanel.h"
#include "MistspireVRPawn.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/CoreStyle.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/EngineVersion.h"

void SMistspireTitlePanel::Construct(const FArguments& InArgs)
{
	OwnerPawn = InArgs._OwnerPawn;

	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 42);
	const FSlateFontInfo SubFont = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	const FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	const FSlateFontInfo BodyFont = FCoreStyle::GetDefaultFontStyle("Regular", 14);

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(36.f))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.55f))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(420.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0.f, 0.f, 0.f, 8.f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("Mistspire", "TitleBrand", "Project Mistspire"))
					.Font(TitleFont)
					.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0.f, 0.f, 0.f, 28.f)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("Mistspire", "TitleDemo", "Demo"))
					.Font(SubFont)
					.ColorAndOpacity(FLinearColor(0.78f, 0.84f, 0.95f, 0.95f))
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					.Visibility(this, &SMistspireTitlePanel::GetMenuVisibility)
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.OnClicked(this, &SMistspireTitlePanel::OnStartClicked)
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("Mistspire", "TitleStart", "Start"))
							.Font(ButtonFont)
						]
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.OnClicked(this, &SMistspireTitlePanel::OnSettingsClicked)
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("Mistspire", "TitleSettings", "Settings"))
							.Font(ButtonFont)
						]
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.OnClicked(this, &SMistspireTitlePanel::OnCreditsClicked)
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("Mistspire", "TitleCredits", "Credits"))
							.Font(ButtonFont)
						]
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.Visibility_Lambda([]()
						{
							return GIsEditor ? EVisibility::Collapsed : EVisibility::Visible;
						})
						.OnClicked(this, &SMistspireTitlePanel::OnQuitClicked)
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("Mistspire", "TitleQuit", "Quit"))
							.Font(ButtonFont)
						]
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 18.f, 0.f, 0.f).HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("Mistspire", "TitleAnyKey", "or press any key to start"))
						.Font(BodyFont)
						.ColorAndOpacity(FLinearColor(0.65f, 0.68f, 0.72f, 0.9f))
					]
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SVerticalBox)
					.Visibility(this, &SMistspireTitlePanel::GetCreditsVisibility)
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 12.f)
					[
						SNew(STextBlock)
						.Text(this, &SMistspireTitlePanel::GetCreditsBody)
						.Font(BodyFont)
						.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.92f))
						.AutoWrapText(true)
					]
					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.OnClicked(this, &SMistspireTitlePanel::OnCreditsClicked)
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("Mistspire", "TitleCreditsBack", "Back"))
							.Font(ButtonFont)
						]
					]
				]
			]
		]
	];
}

FReply SMistspireTitlePanel::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (bShowCredits)
		{
			bShowCredits = false;
			return FReply::Handled();
		}
		return OnSettingsClicked();
	}
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

FReply SMistspireTitlePanel::OnStartClicked()
{
	if (AMistspireVRPawn* Pawn = OwnerPawn.Get())
	{
		Pawn->StartGameplay();
	}
	return FReply::Handled();
}

FReply SMistspireTitlePanel::OnSettingsClicked()
{
	if (AMistspireVRPawn* Pawn = OwnerPawn.Get())
	{
		Pawn->OpenSettingsMenu();
	}
	return FReply::Handled();
}

FReply SMistspireTitlePanel::OnCreditsClicked()
{
	bShowCredits = !bShowCredits;
	return FReply::Handled();
}

FReply SMistspireTitlePanel::OnQuitClicked()
{
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

EVisibility SMistspireTitlePanel::GetMenuVisibility() const
{
	return bShowCredits ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility SMistspireTitlePanel::GetCreditsVisibility() const
{
	return bShowCredits ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SMistspireTitlePanel::GetCreditsBody() const
{
	const FString Engine = FEngineVersion::Current().ToString();
	return FText::FromString(FString::Printf(
		TEXT("Project Mistspire\nA Deepiri demo — climb toward the zenith.\n\nBuilt with Unreal Engine %s\nTeam Deepiri\n\nReach the highest altitude. Survive the biomes."),
		*Engine));
}
