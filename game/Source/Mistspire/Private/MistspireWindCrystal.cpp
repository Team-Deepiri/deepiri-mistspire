#include "MistspireWindCrystal.h"
#include "MistspireVRPawn.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AMistspireWindCrystal::AMistspireWindCrystal()
{
	PrimaryActorTick.bCanEverTick = true;

	CrystalSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CrystalSphere"));
	SetRootComponent(CrystalSphere);
	CrystalSphere->InitSphereRadius(35.f);
	CrystalSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	CrystalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrystalMesh"));
	CrystalMesh->SetupAttachment(CrystalSphere);
}

void AMistspireWindCrystal::BeginPlay()
{
	Super::BeginPlay();
	CrystalSphere->OnComponentBeginOverlap.AddDynamic(this, &AMistspireWindCrystal::OnCrystalOverlap);

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->RegisterInteractiveActor(this);
	}
}

void AMistspireWindCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const float Pulse = 0.85f + 0.15f * FMath::Sin(GetWorld()->GetTimeSeconds() * 3.f);
	SetActorScale3D(FVector(Pulse));
}

void AMistspireWindCrystal::OnCrystalOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Other);
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}

	Pawn->ApplyWindCrystalBoost(GliderBoostSeconds, StaminaRestore);

	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		XR->TriggerHapticVibration(true, 0.5f, 0.12f, 60.f);
		XR->TriggerHapticVibration(false, 0.5f, 0.12f, 60.f);
	}

	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(NSLOCTEXT("Mistspire", "WindCrystal", "Wind crystal surges — ride the updraft."), 3.f);
	}

	Destroy();
}
