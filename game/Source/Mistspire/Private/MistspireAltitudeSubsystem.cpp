#include "MistspireAltitudeSubsystem.h"
#include "MistspireProgressSubsystem.h"

void UMistspireAltitudeSubsystem::ApplyPersonalBest(float AltitudeCm)
{
	PersonalBestAltitudeCm = FMath::Max(PersonalBestAltitudeCm, AltitudeCm);
}

void UMistspireAltitudeSubsystem::UpdateAltitudeFromWorldLocation(const FVector& WorldLocation)
{
	CurrentAltitudeCm = WorldLocation.Z;
	if (CurrentAltitudeCm > PersonalBestAltitudeCm)
	{
		PersonalBestAltitudeCm = CurrentAltitudeCm;
		OnAltitudeRecord.Broadcast(CurrentAltitudeCm, PersonalBestAltitudeCm);

		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
			{
				Progress->CaptureProgressFromWorld(GetWorld());
			}
		}
	}

}
