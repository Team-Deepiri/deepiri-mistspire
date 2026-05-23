#include "MistspirePOIMarker.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireVRPawn.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"

AMistspirePOIMarker::AMistspirePOIMarker()
{
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* DiscoverSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DiscoverSphere"));
	SetRootComponent(DiscoverSphere);
	DiscoverSphere->InitSphereRadius(DiscoverRadiusCm);
	DiscoverSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	POILight = CreateDefaultSubobject<UPointLightComponent>(TEXT("POILight"));
	POILight->SetupAttachment(DiscoverSphere);
	POILight->SetIntensity(1500.f);
	POILight->SetAttenuationRadius(600.f);

	POILabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("POILabel"));
	POILabel->SetupAttachment(DiscoverSphere);
	POILabel->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	POILabel->SetHorizontalAlignment(EHTA_Center);
	POILabel->SetWorldSize(32.f);
}

void AMistspirePOIMarker::BeginPlay()
{
	Super::BeginPlay();

	if (USphereComponent* Sphere = Cast<USphereComponent>(GetRootComponent()))
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMistspirePOIMarker::OnDiscoverOverlap);
	}

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->RegisterInteractiveActor(this);
	}

	if (!POIId.IsNone())
	{
		if (UMistspireWorldAtlasSubsystem* Atlas = GetWorld()->GetSubsystem<UMistspireWorldAtlasSubsystem>())
		{
			for (const FMistspirePOIEntry& P : Atlas->GetPOIs())
			{
				if (P.POIId == POIId)
				{
					SetActorLocation(P.WorldLocation);
					POILabel->SetText(P.Title);
					break;
				}
			}
		}
	}
}

void AMistspirePOIMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bDiscovered && POILight)
	{
		const float Pulse = 0.7f + 0.3f * FMath::Sin(GetWorld()->GetTimeSeconds() * 2.5f);
		POILight->SetIntensity(1200.f * Pulse);
	}
}

void AMistspirePOIMarker::OnDiscoverOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDiscovered)
	{
		return;
	}

	AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Other);
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}

	bDiscovered = true;

	if (UMistspireWorldAtlasSubsystem* Atlas = GetWorld()->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		for (const FMistspirePOIEntry& P : Atlas->GetPOIs())
		{
			if (P.POIId == POIId)
			{
				if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
				{
					Narr->OnPOIDiscovered(P);
				}
				break;
			}
		}
	}
}
