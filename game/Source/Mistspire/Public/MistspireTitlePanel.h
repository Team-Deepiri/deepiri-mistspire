#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class AMistspireVRPawn;

DECLARE_DELEGATE(FOnMistspireTitleClosed);

/** Non-VR title screen: Start / Settings / Credits / Quit. */
class SMistspireTitlePanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMistspireTitlePanel) {}
		SLATE_ARGUMENT(TWeakObjectPtr<AMistspireVRPawn>, OwnerPawn)
	SLATE_END_ARGS()

	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<AMistspireVRPawn> OwnerPawn;
	bool bShowCredits = false;

	FReply OnStartClicked();
	FReply OnSettingsClicked();
	FReply OnCreditsClicked();
	FReply OnQuitClicked();

	EVisibility GetMenuVisibility() const;
	EVisibility GetCreditsVisibility() const;
	FText GetCreditsBody() const;
};
