#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireDialogueSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMistspireDialogueLine, FName, LineId, FText, Speaker, FText, Text);

/** A single dialogue row, usable as a UDataTable row or authored in code. */
USTRUCT(BlueprintType)
struct FDialogueLine : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	float DisplaySeconds = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bAmbient = false;
};

/** Data-driven dialogue queue (Dialogic / Dialogue Manager equivalent). */
UCLASS()
class MISTSPIRE_API UMistspireDialogueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Plays a line by id: DataTable row first, then the built-in line set. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Dialogue")
	void Speak(FName LineId);

	/** Plays an ad-hoc line (e.g. runtime-generated flavor). */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Dialogue")
	void SpeakText(const FText& Speaker, const FText& Text, float DisplaySeconds = 5.f);

	/** Overrides the built-in set with an authored DataTable of FDialogueLine rows. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Dialogue")
	void SetDialogueTable(UDataTable* InTable);

	/** Looks up a line by id: DataTable row first, then the built-in line set. */
	const FDialogueLine* FindLine(FName LineId) const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Dialogue")
	FName GetLastLineId() const { return LastLineId; }

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Dialogue")
	FOnMistspireDialogueLine OnDialogueLine;

private:
	void LoadBuiltinLines();

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> DialogueTable;

	TMap<FName, FDialogueLine> BuiltinLines;
	FName LastLineId = NAME_None;
};