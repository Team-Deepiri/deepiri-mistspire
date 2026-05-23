#include "MistspireGhostPillar.h"
#include "Components/StaticMeshComponent.h"

AMistspireGhostPillar::AMistspireGhostPillar()
{
	PrimaryActorTick.bCanEverTick = false;

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	SetRootComponent(PillarMesh);
	PillarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PillarMesh->SetCastShadow(false);
	PillarMesh->SetRenderCustomDepth(true);
	PillarMesh->SetCustomDepthStencilValue(2);
}

void AMistspireGhostPillar::Configure(const FVector& WorldLocation, float HeightCm, const FString& Label)
{
	const float PillarHeight = FMath::Clamp(HeightCm, 500.f, 2000000.f);
	SetActorLocation(WorldLocation);
	SetActorScale3D(FVector(0.2f, 0.2f, PillarHeight / 10000.f));
	SetActorLabel(FString::Printf(TEXT("Ghost_%s"), *Label));
}
