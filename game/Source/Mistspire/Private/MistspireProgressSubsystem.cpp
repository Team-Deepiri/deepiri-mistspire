#include "MistspireProgressSubsystem.h"
#include "MistspireSaveGame.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireSummitRegistry.h"
#include "MistspirePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

const FString UMistspireProgressSubsystem::SaveSlotName(TEXT("MistspireProfile"));

void UMistspireProgressSubsystem::LoadProgress()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (UMistspireSaveGame* Save = Cast<UMistspireSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
		{
			CachedPersonalBestCm = Save->PersonalBestAltitudeCm;
			CachedSummits = Save->ReachedSummits;
			bLoaded = true;
			return;
		}
	}
	bLoaded = true;
}

void UMistspireProgressSubsystem::SaveProgress()
{
	UMistspireSaveGame* Save = Cast<UMistspireSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UMistspireSaveGame::StaticClass()));
	if (!Save)
	{
		return;
	}

	Save->PersonalBestAltitudeCm = CachedPersonalBestCm;
	Save->ReachedSummits = CachedSummits;
	Save->TotalSessions += 1;
	UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);
}

void UMistspireProgressSubsystem::ApplyLoadedProgressToWorld(UWorld* World)
{
	if (!World || !bLoaded)
	{
		return;
	}

	if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
	{
		Alt->ApplyPersonalBest(CachedPersonalBestCm);
	}

	if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
	{
		Registry->ApplyReachedSummits(CachedSummits);
	}

	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (AMistspirePlayerState* PS = PC->GetPlayerState<AMistspirePlayerState>())
		{
			PS->ApplyLoadedProgress(CachedPersonalBestCm, CachedSummits);
		}
	}
}

void UMistspireProgressSubsystem::CaptureProgressFromWorld(UWorld* World)
{
	if (!World)
	{
		return;
	}

	if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
	{
		CachedPersonalBestCm = FMath::Max(CachedPersonalBestCm, Alt->GetPersonalBestAltitudeCm());
	}

	CachedSummits.Reset();
	if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
	{
		for (const FMistspireSummitEntry& Entry : Registry->GetSummits())
		{
			if (Entry.bReached)
			{
				CachedSummits.Add(Entry.SummitId);
			}
		}
	}

	if (APlayerController* PC = World->GetFirstPlayerController())
	{
		if (AMistspirePlayerState* PS = PC->GetPlayerState<AMistspirePlayerState>())
		{
			CachedPersonalBestCm = FMath::Max(CachedPersonalBestCm, PS->MaxAltitudeCm);
			for (FName Id : PS->ReachedSummits)
			{
				CachedSummits.AddUnique(Id);
			}
		}
	}

	SaveProgress();
}
