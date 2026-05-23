#include "MistspireInteriorSubsystem.h"
#include "MistspireBuildingEntrance.h"
#include "MistspireVRPawn.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireXRActionSubsystem.h"

bool UMistspireInteriorSubsystem::EnterBuilding(AMistspireVRPawn* Pawn, FName BuildingId, const FVector& InReturnLocation, const FRotator& InReturnRotation)
{
	if (!Pawn || BuildingId.IsNone() || bInsideInterior)
	{
		return false;
	}

	UMistspireWorldAtlasSubsystem* Atlas = GetWorld()->GetSubsystem<UMistspireWorldAtlasSubsystem>();
	if (!Atlas)
	{
		return false;
	}

	FMistspireBuildingEntry Entry;
	if (!Atlas->FindBuilding(BuildingId, Entry) || !Entry.bHasInterior)
	{
		return false;
	}

	ReturnLocation = InReturnLocation;
	ReturnRotation = InReturnRotation;
	CurrentBuildingId = BuildingId;
	bInsideInterior = true;

	Pawn->SetActorLocationAndRotation(Entry.InteriorSpawnLocation, Entry.InteriorSpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);

	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		XR->TriggerHapticVibration(true, 0.2f, 0.06f, 90.f);
		XR->TriggerHapticVibration(false, 0.2f, 0.06f, 90.f);
	}

	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		if (!Entry.EnterLine.IsEmpty())
		{
			Narr->PushLine(Entry.EnterLine, 5.f);
		}
		Narr->OnBuildingEntered(BuildingId, Entry.DisplayName);
	}

	OnInteriorChanged.Broadcast(BuildingId, true);
	return true;
}

bool UMistspireInteriorSubsystem::EnterBuildingFromEntrance(AMistspireVRPawn* Pawn, AMistspireBuildingEntrance* Entrance)
{
	if (!Pawn || !Entrance)
	{
		return false;
	}

	const FVector ReturnLoc = Entrance->GetActorLocation() + Entrance->GetActorForwardVector() * 120.f;
	return EnterBuilding(Pawn, Entrance->GetBuildingId(), ReturnLoc, Entrance->GetActorRotation());
}

void UMistspireInteriorSubsystem::ExitBuilding(AMistspireVRPawn* Pawn)
{
	if (!Pawn || !bInsideInterior)
	{
		return;
	}

	const FName ExitedId = CurrentBuildingId;
	Pawn->SetActorLocationAndRotation(ReturnLocation, ReturnRotation, false, nullptr, ETeleportType::TeleportPhysics);

	bInsideInterior = false;
	CurrentBuildingId = NAME_None;

	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		XR->TriggerHapticVibration(true, 0.15f, 0.05f, 70.f);
	}

	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(NSLOCTEXT("Mistspire", "ExitBuilding", "Back into the open air."), 3.f);
	}

	OnInteriorChanged.Broadcast(ExitedId, false);
}
