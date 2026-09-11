#include "MistspireInteractionSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireInputMode.h"
#include "MistspireInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"

void UMistspireInteractionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(PlayerPawn);
	if (!VRPawn)
	{
		return;
	}

	const bool bNonVR = FMistspireInputMode::IsNonVRMode(GetWorld());

	TArray<FVector> ProximityPoints;
	GatherProximityPoints(VRPawn, ProximityPoints, bNonVR);

	const float ProximityCm = bNonVR ? 120.f : 35.f;
	UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>();

	for (AActor* Actor : InteractiveActors)
	{
		if (!Actor)
		{
			continue;
		}

		bool bShouldHighlight = false;
		for (const FVector& Point : ProximityPoints)
		{
			if (FVector::Dist(Actor->GetActorLocation(), Point) < ProximityCm)
			{
				bShouldHighlight = true;
				break;
			}
		}

		const bool bWasHighlighted = HighlightStates.FindRef(Actor);
		if (bWasHighlighted != bShouldHighlight)
		{
			HighlightStates.Add(Actor, bShouldHighlight);

			TArray<UPrimitiveComponent*> Comps;
			Actor->GetComponents<UPrimitiveComponent>(Comps);
			for (UPrimitiveComponent* Comp : Comps)
			{
				Comp->SetRenderCustomDepth(bShouldHighlight);
				Comp->SetCustomDepthStencilValue(bShouldHighlight ? 1 : 0);
			}

			if (bShouldHighlight && XR && VRPawn->IsLocallyControlled() && !bNonVR)
			{
				XR->TriggerHapticVibration(true, 0.12f, 0.04f, 90.f);
			}
		}
	}
}

void UMistspireInteractionSubsystem::TryInteractFromPawn(AMistspireVRPawn* Pawn)
{
	if (!Pawn || !FMistspireInputMode::IsNonVRMode(GetWorld()))
	{
		return;
	}

	const FVector Start = Pawn->GetInteractionTraceStart();
	const FVector End = Pawn->GetInteractionTraceEnd(400.f);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MistspireInteract), false, Pawn);
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params) || !Hit.GetActor())
	{
		return;
	}

	if (Hit.GetActor()->GetClass()->ImplementsInterface(UMistspireInteractable::StaticClass()))
	{
		IMistspireInteractable::Execute_MistspireInteract(Hit.GetActor(), Pawn);
	}
}

void UMistspireInteractionSubsystem::GatherProximityPoints(
	AMistspireVRPawn* Pawn, TArray<FVector>& OutPoints, bool bNonVRMode) const
{
	OutPoints.Reset();
	OutPoints.Add(Pawn->GetActorLocation());
	OutPoints.Add(Pawn->GetLeftHandWorldLocation());
	OutPoints.Add(Pawn->GetRightHandWorldLocation());
	if (bNonVRMode)
	{
		OutPoints.Add(Pawn->GetInteractionTraceEnd(80.f));
	}
}

TStatId UMistspireInteractionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireInteractionSubsystem, STATGROUP_Tickables);
}

void UMistspireInteractionSubsystem::RegisterInteractiveActor(AActor* Actor)
{
	InteractiveActors.AddUnique(Actor);
}

void UMistspireInteractionSubsystem::UnregisterInteractiveActor(AActor* Actor)
{
	InteractiveActors.Remove(Actor);
	HighlightStates.Remove(Actor);
}
