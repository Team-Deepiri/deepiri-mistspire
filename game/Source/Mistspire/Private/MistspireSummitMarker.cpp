#include "MistspireSummitMarker.h"
#include "MistspireSummitRegistry.h"
#include "MistspireInteractionSubsystem.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"

AMistspireSummitMarker::AMistspireSummitMarker()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BeaconLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeaconLight"));
	BeaconLight->SetupAttachment(Root);
	BeaconLight->SetLightColor(FLinearColor(0.6f, 0.9f, 1.f));
	BeaconLight->SetIntensity(4000.f);
	BeaconLight->SetAttenuationRadius(1200.f);

	SummitLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SummitLabel"));
	SummitLabel->SetupAttachment(Root);
	SummitLabel->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	SummitLabel->SetHorizontalAlignment(EHTA_Center);
	SummitLabel->SetWorldSize(80.f);

#if WITH_EDITORONLY_DATA
	if (UBillboardComponent* S = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite")))
	{
		S->SetupAttachment(Root);
	}
#endif
}

void AMistspireSummitMarker::BeginPlay()
{
	Super::BeginPlay();

	if (!SummitId.IsNone())
	{
		if (UMistspireSummitRegistry* R = GetWorld()->GetSubsystem<UMistspireSummitRegistry>())
		{
			const float Alt = OfficialAltitudeCm > 0.f ? OfficialAltitudeCm : GetActorLocation().Z;
			R->RegisterSummit(SummitId, GetActorLocation(), Alt, ReachRadiusCm);
		}

		FString Label = SummitId.ToString();
		Label.RemoveFromStart(TEXT("summit_"));
		Label.ReplaceInline(TEXT("_"), TEXT(" "));
		if (SummitLabel)
		{
			SummitLabel->SetText(FText::FromString(Label.ToUpper()));
		}
	}

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->RegisterInteractiveActor(this);
	}
}

void AMistspireSummitMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const float Pulse = 0.65f + 0.35f * FMath::Sin(GetWorld()->GetTimeSeconds() * 2.2f);
	if (BeaconLight)
	{
		BeaconLight->SetIntensity(2500.f + 3500.f * Pulse);
	}
}
