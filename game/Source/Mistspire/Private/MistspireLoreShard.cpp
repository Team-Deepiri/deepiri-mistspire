#include "MistspireLoreShard.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireInputMode.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AMistspireLoreShard::AMistspireLoreShard()
{
	PrimaryActorTick.bCanEverTick = false;

	ShardSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ShardSphere"));
	SetRootComponent(ShardSphere);
	ShardSphere->InitSphereRadius(25.f);
	ShardSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	ShardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShardMesh"));
	ShardMesh->SetupAttachment(ShardSphere);
}

void AMistspireLoreShard::BeginPlay()
{
	Super::BeginPlay();
	ShardSphere->OnComponentBeginOverlap.AddDynamic(this, &AMistspireLoreShard::OnShardOverlap);

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->RegisterInteractiveActor(this);
	}
}

void AMistspireLoreShard::OnShardOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Other))
	{
		CollectShard(Pawn);
	}
}

void AMistspireLoreShard::MistspireInteract_Implementation(AActor* Instigator)
{
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(Instigator))
	{
		CollectShard(Pawn);
	}
}

void AMistspireLoreShard::CollectShard(AMistspireVRPawn* Pawn)
{
	if (bCollected || !Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}
	bCollected = true;

	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(FText::Format(
			NSLOCTEXT("Mistspire", "LoreShard", "{0} — {1}"),
			LoreTitle, LoreBody), 8.f);
	}

	if (!FMistspireInputMode::IsNonVRMode(GetWorld()))
	{
		if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			XR->TriggerHapticVibration(true, 0.2f, 0.08f, 110.f);
		}
	}

	Destroy();
}
