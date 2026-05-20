#include "MistspireInteractionSubsystem.h"
#include "MistspireVRPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"

void UMistspireInteractionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(PlayerPawn);
	if (!VRPawn) return;

	// Use hand locations from the pawn
	TArray<FVector> HandLocations;
	HandLocations.Add(VRPawn->GetActorLocation()); // Fallback to capsule center

	for (AActor* Actor : InteractiveActors)
	{
		if (!Actor) continue;

		bool bShouldHighlight = false;
		float MinDist = 30.f; // 30cm proximity for highlight

		float Dist = FVector::Dist(Actor->GetActorLocation(), VRPawn->GetActorLocation());
		if (Dist < MinDist)
		{
			bShouldHighlight = true;
		}

		if (HighlightStates.FindOrAdd(Actor) != bShouldHighlight)
		{
			HighlightStates[Actor] = bShouldHighlight;
			
			// Set custom depth on all primitive components for outline effect
			TArray<UPrimitiveComponent*> Comps;
			Actor->GetComponents<UPrimitiveComponent>(Comps);
			for (UPrimitiveComponent* Comp : Comps)
			{
				Comp->SetRenderCustomDepth(bShouldHighlight);
				Comp->SetCustomDepthStencilValue(1); // Standard interaction color
			}
		}
	}
}

TStatId UMistspireInteractionSubsystem::GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireInteractionSubsystem, STATGROUP_Tickables); }

void UMistspireInteractionSubsystem::RegisterInteractiveActor(AActor* Actor)
{
	InteractiveActors.AddUnique(Actor);
}

void UMistspireInteractionSubsystem::UnregisterInteractiveActor(AActor* Actor)
{
	InteractiveActors.Remove(Actor);
	HighlightStates.Remove(Actor);
}
