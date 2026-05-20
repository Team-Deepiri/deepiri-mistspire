#include "MistspireSummitMarker.h"
#include "MistspireSummitRegistry.h"
#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"

AMistspireSummitMarker::AMistspireSummitMarker()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
#if WITH_EDITORONLY_DATA
	if (UBillboardComponent* S = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite")))
		S->SetupAttachment(Root);
#endif
}

void AMistspireSummitMarker::BeginPlay()
{
	Super::BeginPlay();
	if (!SummitId.IsNone() && GetWorld())
		if (UMistspireSummitRegistry* R = GetWorld()->GetSubsystem<UMistspireSummitRegistry>())
			R->RegisterSummit(SummitId, GetActorLocation(), OfficialAltitudeCm);
}
