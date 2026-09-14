#include "MistspireInteriorSubsystem.h"
#include "MistspireBuildingEntrance.h"
#include "MistspireVRPawn.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireDemoSpireLayout.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

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

	// Trust the caller's XY and the pawn's current standing Z. A world-static line trace was
	// hitting the still-colliding OpenWorld landscape (hidden but not collision-disabled) and
	// other high blockers, which stored a sky Z and dropped you above the inn on exit.
	ReturnLocation = InReturnLocation;
	ReturnRotation = InReturnRotation;
	CurrentBuildingId = BuildingId;
	bInsideInterior = true;

	UE_LOG(LogTemp, Log,
		TEXT("Mistspire Interior: enter %s — stored return (%.0f, %.0f, %.0f) (pawn was at Z=%.0f)"),
		*BuildingId.ToString(),
		ReturnLocation.X, ReturnLocation.Y, ReturnLocation.Z,
		Pawn->GetActorLocation().Z);

	// Drop motion + ground cache before the pocket teleport. The Mist Inn floor sits at Z≈20 km;
	// leaving NonVRCachedSupportZ at that height makes SnapFeetToGround lerp the pawn into the
	// sky on the way back out (stationary stick / walk interp both reuse the cached Z).
	Pawn->ResetMotionForDebugTeleport();
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

	// Ignore enter overlaps briefly after an exit so the porch return cannot re-trigger the door.
	if (GetWorld() && GetWorld()->GetTimeSeconds() < EnterIgnoreUntilTime)
	{
		return false;
	}

	FVector ReturnLoc;
	FRotator ReturnRot = Entrance->GetActorRotation();
	ReturnRot.Pitch = 0.f;
	ReturnRot.Roll = 0.f;

	// Platform height = where the capsule center is right now (they are standing on the porch).
	const float StandingPawnZ = Pawn->GetActorLocation().Z;

	if (Entrance->GetBuildingId() == FName(TEXT("building_valley_inn")))
	{
		// Layout XY is authoritative; Z prefers the live porch stance but rejects anything that
		// is clearly not shelf-height (e.g. enter while already mid-air / after a bad snap).
		ReturnLoc = MistspireDemoSpire::GetMistInnExitReturnLocation();
		const float ShelfZ = ReturnLoc.Z;
		ReturnLoc.Z = (FMath::Abs(StandingPawnZ - ShelfZ) <= 400.f) ? StandingPawnZ : ShelfZ;
		ReturnRot = FRotator(0.f, 90.f, 0.f);
	}
	else
	{
		const FVector FlatForward = Entrance->GetActorForwardVector().GetSafeNormal2D();
		ReturnLoc = Entrance->GetActorLocation() + FlatForward * 250.f;
		ReturnLoc.Z = StandingPawnZ;
	}

	return EnterBuilding(Pawn, Entrance->GetBuildingId(), ReturnLoc, ReturnRot);
}

void UMistspireInteriorSubsystem::ExitBuilding(AMistspireVRPawn* Pawn)
{
	if (!Pawn || !bInsideInterior)
	{
		return;
	}

	const FName ExitedId = CurrentBuildingId;

	UE_LOG(LogTemp, Log,
		TEXT("Mistspire Interior: exit %s → (%.0f, %.0f, %.0f)"),
		*ExitedId.ToString(), ReturnLocation.X, ReturnLocation.Y, ReturnLocation.Z);

	bInsideInterior = false;
	CurrentBuildingId = NAME_None;
	EnterIgnoreUntilTime = GetWorld() ? GetWorld()->GetTimeSeconds() + 1.25f : 0.f;

	// Clear the pocket-floor ground cache BEFORE returning to the porch. Without this, the next
	// non-VR SnapFeetToGround frame still holds NonVRCachedSupportZ from Z≈20000 and walks the
	// capsule back up into the sky even though ReturnLocation.Z is correct (see log: exit → 152).
	Pawn->ResetMotionForDebugTeleport();
	Pawn->SetActorLocationAndRotation(ReturnLocation, ReturnRotation, false, nullptr, ETeleportType::TeleportPhysics);

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
