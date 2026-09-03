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
	bReplicates = true;

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

void AMistspireLoreShard::MistspireInteract_Implementation(AActor* InteractInstigator)
{
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(InteractInstigator))
	{
		CollectShard(Pawn);
	}
}

void AMistspireLoreShard::CollectShard(AMistspireVRPawn* Pawn)
{
	if (bCollected || !Pawn)
	{
		return;
	}

	if (HasAuthority())
	{
		ApplyCollection(Pawn);
	}
	else if (Pawn->IsLocallyControlled())
	{
		// Route through the owned pawn — this actor has no owning connection.
		Pawn->Server_CollectLoreShard(this);
	}
}

void AMistspireLoreShard::ApplyCollection(AMistspireVRPawn* Pawn)
{
	if (bCollected || !Pawn || !HasAuthority())
	{
		return;
	}

	bCollected = true;
	Pawn->DeliverLoreShard(LoreTitle, LoreBody);

	if (UMistspireInteractionSubsystem* Sub = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Sub->UnregisterInteractiveActor(this);
	}

	Destroy();
}
