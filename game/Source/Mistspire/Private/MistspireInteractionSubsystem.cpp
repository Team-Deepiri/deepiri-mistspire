#include "MistspireInteractionSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireXRActionSubsystem.h"
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

	const TArray<FVector> HandLocations = {
		VRPawn->GetLeftHandWorldLocation(),
		VRPawn->GetRightHandWorldLocation(),
		VRPawn->GetActorLocation()
	};

	const float ProximityCm = 35.f;
	UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>();

	for (AActor* Actor : InteractiveActors)
	{
		if (!Actor)
		{
			continue;
		}

		bool bShouldHighlight = false;
		for (const FVector& HandLoc : HandLocations)
		{
			if (FVector::Dist(Actor->GetActorLocation(), HandLoc) < ProximityCm)
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

			if (bShouldHighlight && XR && VRPawn->IsLocallyControlled())
			{
				XR->TriggerHapticVibration(true, 0.12f, 0.04f, 90.f);
			}
		}
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
