#include "MistspireBuildingEntrance.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireVRPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"

AMistspireBuildingEntrance::AMistspireBuildingEntrance()
{
	PrimaryActorTick.bCanEverTick = false;

	DoorVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorVolume"));
	SetRootComponent(DoorVolume);
	DoorVolume->SetBoxExtent(FVector(80.f, 120.f, 200.f));
	DoorVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	DoorFrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	DoorFrameMesh->SetupAttachment(DoorVolume);

	BuildingSign = CreateDefaultSubobject<UTextRenderComponent>(TEXT("BuildingSign"));
	BuildingSign->SetupAttachment(DoorVolume);
	BuildingSign->SetRelativeLocation(FVector(0.f, 0.f, 240.f));
	BuildingSign->SetHorizontalAlignment(EHTA_Center);
	BuildingSign->SetWorldSize(24.f);
}

void AMistspireBuildingEntrance::BeginPlay()
{
	Super::BeginPlay();

	DoorVolume->OnComponentBeginOverlap.AddDynamic(this, &AMistspireBuildingEntrance::OnDoorOverlap);

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->RegisterInteractiveActor(this);
	}

	if (!BuildingId.IsNone())
	{
		if (UMistspireWorldAtlasSubsystem* Atlas = GetWorld()->GetSubsystem<UMistspireWorldAtlasSubsystem>())
		{
			FMistspireBuildingEntry Entry;
			if (Atlas->FindBuilding(BuildingId, Entry))
			{
				BuildingSign->SetText(Entry.DisplayName);
				SetActorLocation(Entry.ExteriorDoorLocation);
				SetActorRotation(Entry.ExteriorDoorRotation);
			}
		}
	}
}

void AMistspireBuildingEntrance::OnDoorOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Other);
	if (!Pawn || !Pawn->IsLocallyControlled() || BuildingId.IsNone())
	{
		return;
	}

	if (UMistspireInteriorSubsystem* Interior = GetWorld()->GetSubsystem<UMistspireInteriorSubsystem>())
	{
		if (Interior->IsInsideInterior())
		{
			return;
		}
		Interior->EnterBuildingFromEntrance(Pawn, this);
	}
}
