#include "MistspireOxygenCanister.h"
#include "MistspireVRPawn.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AMistspireOxygenCanister::AMistspireOxygenCanister()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	SetRootComponent(PickupSphere);
	PickupSphere->InitSphereRadius(28.f);
	PickupSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	CanisterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CanisterMesh"));
	CanisterMesh->SetupAttachment(PickupSphere);
}

void AMistspireOxygenCanister::BeginPlay()
{
	Super::BeginPlay();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AMistspireOxygenCanister::OnPickupOverlap);

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->RegisterInteractiveActor(this);
	}
}

void AMistspireOxygenCanister::OnPickupOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Other);
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}

	Pawn->ApplyShelterRefill(OxygenRestore, 0.f, 1.f);

	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		XR->TriggerHapticVibration(false, 0.35f, 0.08f, 140.f);
	}

	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(NSLOCTEXT("Mistspire", "O2Canister", "Pressurized O₂ — lungs steady."), 3.f);
	}

	if (bConsumeOnPickup)
	{
		Destroy();
	}
}
