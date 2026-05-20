#include "MistspireAltitudeSubsystem.h"
#include "MistspireGameState.h"

void UMistspireAltitudeSubsystem::UpdateAltitudeFromWorldLocation(const FVector& WorldLocation)
{
	CurrentAltitudeCm = WorldLocation.Z;
	if (CurrentAltitudeCm > PersonalBestAltitudeCm)
	{
		PersonalBestAltitudeCm = CurrentAltitudeCm;
		OnAltitudeRecord.Broadcast(CurrentAltitudeCm, PersonalBestAltitudeCm);
	}

	if (UWorld* World = GetWorld())
	{
		if (AMistspireGameState* GS = World->GetGameState<AMistspireGameState>())
		{
			GS->NotifyAltitudeSample(CurrentAltitudeCm);
		}
	}
}
