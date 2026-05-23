#include "MistspireInteriorExit.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireVRPawn.h"
#include "Components/BoxComponent.h"

AMistspireInteriorExit::AMistspireInteriorExit()
{
	PrimaryActorTick.bCanEverTick = false;

	ExitVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ExitVolume"));
	SetRootComponent(ExitVolume);
	ExitVolume->SetBoxExtent(FVector(100.f, 100.f, 200.f));
	ExitVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AMistspireInteriorExit::BeginPlay()
{
	Super::BeginPlay();
	ExitVolume->OnComponentBeginOverlap.AddDynamic(this, &AMistspireInteriorExit::OnExitOverlap);
}

void AMistspireInteriorExit::OnExitOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Other);
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}

	if (UMistspireInteriorSubsystem* Interior = GetWorld()->GetSubsystem<UMistspireInteriorSubsystem>())
	{
		if (Interior->IsInsideInterior())
		{
			Interior->ExitBuilding(Pawn);
		}
	}
}
