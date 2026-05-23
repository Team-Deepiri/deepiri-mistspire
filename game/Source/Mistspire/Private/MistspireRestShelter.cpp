#include "MistspireRestShelter.h"
#include "MistspireVRPawn.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireXRActionSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"

AMistspireRestShelter::AMistspireRestShelter()
{
	PrimaryActorTick.bCanEverTick = false;

	ShelterVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ShelterVolume"));
	SetRootComponent(ShelterVolume);
	ShelterVolume->SetBoxExtent(FVector(RefillRadiusCm));
	ShelterVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	ShelterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShelterMesh"));
	ShelterMesh->SetupAttachment(ShelterVolume);

	WarmLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("WarmLight"));
	WarmLight->SetupAttachment(ShelterVolume);
	WarmLight->SetLightColor(FLinearColor(1.f, 0.65f, 0.35f));
	WarmLight->SetIntensity(2500.f);
	WarmLight->SetAttenuationRadius(400.f);
}

void AMistspireRestShelter::BeginPlay()
{
	Super::BeginPlay();

	ShelterVolume->OnComponentBeginOverlap.AddDynamic(this, &AMistspireRestShelter::OnShelterOverlap);

	if (UWorld* World = GetWorld())
	{
		if (UMistspireInteractionSubsystem* Sub = World->GetSubsystem<UMistspireInteractionSubsystem>())
		{
			Sub->RegisterInteractiveActor(this);
		}
	}
}

void AMistspireRestShelter::OnShelterOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(Other);
	if (!VRPawn || !VRPawn->IsLocallyControlled())
	{
		return;
	}

	VRPawn->ApplyShelterRefill(OxygenRefillPerSecond, StaminaRefillPerSecond, 0.5f);

	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		XR->TriggerHapticVibration(true, 0.25f, 0.12f, 80.f);
		XR->TriggerHapticVibration(false, 0.25f, 0.12f, 80.f);
	}

	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(NSLOCTEXT("Mistspire", "Shelter", "Shelter warmth. Breath returns."), 4.f);
	}
}
