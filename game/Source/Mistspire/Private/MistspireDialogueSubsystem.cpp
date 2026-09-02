#include "MistspireDialogueSubsystem.h"
#include "Misc/PackageName.h"

void UMistspireDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadBuiltinLines();

	const FSoftObjectPath DialogueTablePath(TEXT("/Game/Data/DT_MistspireDialogue.DT_MistspireDialogue"));
	if (FPackageName::DoesPackageExist(DialogueTablePath.GetLongPackageName()))
	{
		if (UDataTable* LoadedTable = Cast<UDataTable>(DialogueTablePath.TryLoad()))
		{
			SetDialogueTable(LoadedTable);
		}
	}
}

void UMistspireDialogueSubsystem::LoadBuiltinLines()
{
	auto MakeLine = [](const FString& Speaker, const FString& Text, float Seconds, bool bAmbient)
	{
		FDialogueLine Line;
		Line.Speaker = FText::FromString(Speaker);
		Line.Text = FText::FromString(Text);
		Line.DisplaySeconds = Seconds;
		Line.bAmbient = bAmbient;
		return Line;
	};

	BuiltinLines.Empty();
	BuiltinLines.Add(TEXT("companion_greeting"), MakeLine(
		TEXT("Guide Spirit"), TEXT("Higher. The mist keeps what you leave behind."), 6.f, false));
	BuiltinLines.Add(TEXT("ghost_whisper"), MakeLine(
		TEXT("Ghost"), TEXT("They climbed the same wall. They did not look down."), 5.f, true));
	BuiltinLines.Add(TEXT("summit_breath"), MakeLine(
		TEXT("Guide Spirit"), TEXT("One more handhold. The sky opens above you."), 5.f, false));
	BuiltinLines.Add(TEXT("shelter_warmth"), MakeLine(
		TEXT("Shelter"), TEXT("Still air. The storm cannot reach you here."), 4.f, false));
	BuiltinLines.Add(TEXT("storm_warning"), MakeLine(
		TEXT("Ghost"), TEXT("Lightning hunts the exposed ridge. Find stone."), 5.f, true));
	BuiltinLines.Add(TEXT("zenith_glow"), MakeLine(
		TEXT("Guide Spirit"), TEXT("The summit light drinks the dark. Climb it."), 6.f, false));
	BuiltinLines.Add(TEXT("oxygen_low"), MakeLine(
		TEXT("Guide Spirit"), TEXT("Your breath is thin. Shelter before the mist takes it."), 5.f, false));
}

const FDialogueLine* UMistspireDialogueSubsystem::FindLine(FName LineId) const
{
	if (DialogueTable)
	{
		if (const FDialogueLine* Row = DialogueTable->FindRow<FDialogueLine>(LineId, TEXT("MistspireDialogue"), false))
		{
			return Row;
		}
	}
	return BuiltinLines.Find(LineId);
}

void UMistspireDialogueSubsystem::SetDialogueTable(UDataTable* InTable)
{
	DialogueTable = InTable;
}

void UMistspireDialogueSubsystem::Speak(FName LineId)
{
	if (const FDialogueLine* Line = FindLine(LineId))
	{
		LastLineId = LineId;
		OnDialogueLine.Broadcast(LineId, Line->Speaker, Line->Text);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire dialogue: unknown line '%s'"), *LineId.ToString());
	}
}

void UMistspireDialogueSubsystem::SpeakText(const FText& Speaker, const FText& Text, float DisplaySeconds)
{
	LastLineId = NAME_None;
	OnDialogueLine.Broadcast(NAME_None, Speaker, Text);
}