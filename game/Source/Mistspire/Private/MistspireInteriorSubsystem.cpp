#include "MistspireInteriorSubsystem.h"
#include "MistspireBuildingEntrance.h"
#include "MistspireVRPawn.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireDemoSpireLayout.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"

namespace MistspireInteriorPrivate
{
	float CapsuleHalfHeight(const AMistspireVRPawn* Pawn)
	{
		if (const UCapsuleComponent* Cap = Pawn ? Pawn->FindComponentByClass<UCapsuleComponent>() : nullptr)
		{
			return Cap->GetScaledCapsuleHalfHeight();
		}
		return 88.f;
	}

	/**
	 * Snap a return XY onto whatever walkable surface is actually under it (ShelfPad / porch /
	 * landscape). Guesses from ValleyFloorZ left the pawn in mid-air when the real platform
	 * height disagreed — or inside collision that then ejected them upward.
	 */
	FVector SnapReturnToPlatform(UWorld* World, const FVector& DesiredXY, float CapsuleHalfZ, AActor* IgnoreActor)
	{
		const FVector ProbeXY(DesiredXY.X, DesiredXY.Y, DesiredXY.Z);
		const FVector TraceStart(ProbeXY.X, ProbeXY.Y, ProbeXY.Z + 100000.f);
		const FVector TraceEnd(ProbeXY.X, ProbeXY.Y, ProbeXY.Z - 200000.f);

		FCollisionQueryParams Params(FName(TEXT("MistspireInteriorReturnSnap")), false, IgnoreActor);
		Params.bReturnPhysicalMaterial = false;

		FHitResult Hit;
		if (World && World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
		{
			const float FeetZ = static_cast<float>(Hit.ImpactPoint.Z);
			UE_LOG(LogTemp, Log,
				TEXT("Mistspire Interior: return snap hit %s at Z=%.0f → pawn Z=%.0f"),
				*GetNameSafe(Hit.GetActor()), FeetZ, FeetZ + CapsuleHalfZ);
			return FVector(ProbeXY.X, ProbeXY.Y, FeetZ + CapsuleHalfZ);
		}

		UE_LOG(LogTemp, Warning,
			TEXT("Mistspire Interior: return snap missed at (%.0f,%.0f) — using DesiredXY Z=%.0f"),
			ProbeXY.X, ProbeXY.Y, ProbeXY.Z);
		return ProbeXY;
	}
}

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

	const float HalfZ = MistspireInteriorPrivate::CapsuleHalfHeight(Pawn);
	ReturnLocation = MistspireInteriorPrivate::SnapReturnToPlatform(GetWorld(), InReturnLocation, HalfZ, Pawn);
	ReturnRotation = InReturnRotation;
	CurrentBuildingId = BuildingId;
	bInsideInterior = true;

	UE_LOG(LogTemp, Log,
		TEXT("Mistspire Interior: enter %s — stored return (%.0f, %.0f, %.0f)"),
		*BuildingId.ToString(), ReturnLocation.X, ReturnLocation.Y, ReturnLocation.Z);

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

	// Ignore enter overlaps briefly after an exit so the porch return point cannot immediately
	// re-trigger the door and bounce the pawn back into the pocket (reads as "stuck in the sky").
	if (GetWorld() && GetWorld()->GetTimeSeconds() < EnterIgnoreUntilTime)
	{
		return false;
	}

	FVector ReturnLoc;
	FRotator ReturnRot = Entrance->GetActorRotation();
	ReturnRot.Pitch = 0.f;
	ReturnRot.Roll = 0.f;

	if (Entrance->GetBuildingId() == FName(TEXT("building_valley_inn")))
	{
		// Far enough past the enter volume that the porch drop does not re-overlap the door.
		ReturnLoc = MistspireDemoSpire::GetMistInnExitReturnLocation();
		ReturnRot = FRotator(0.f, 90.f, 0.f);
	}
	else
	{
		const FVector FlatForward = Entrance->GetActorForwardVector().GetSafeNormal2D();
		ReturnLoc = Entrance->GetActorLocation() + FlatForward * 250.f;
		ReturnLoc.Z = Entrance->GetActorLocation().Z;
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
	const float HalfZ = MistspireInteriorPrivate::CapsuleHalfHeight(Pawn);

	// Snap again at exit in case the shelf was rebuilt / landscape streamed while inside.
	const FVector LandAt = MistspireInteriorPrivate::SnapReturnToPlatform(GetWorld(), ReturnLocation, HalfZ, Pawn);

	UE_LOG(LogTemp, Log,
		TEXT("Mistspire Interior: exit %s → (%.0f, %.0f, %.0f)"),
		*ExitedId.ToString(), LandAt.X, LandAt.Y, LandAt.Z);

	bInsideInterior = false;
	CurrentBuildingId = NAME_None;
	EnterIgnoreUntilTime = GetWorld() ? GetWorld()->GetTimeSeconds() + 1.25f : 0.f;

	Pawn->SetActorLocationAndRotation(LandAt, ReturnRotation, false, nullptr, ETeleportType::TeleportPhysics);

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
