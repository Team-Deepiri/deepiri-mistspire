#include "MistspireGuideSpirit.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "MistspireSteering.h"
#include "UObject/ConstructorHelpers.h"

AMistspireGuideSpirit::AMistspireGuideSpirit()
{
	PrimaryActorTick.bCanEverTick = true;

	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	SetRootComponent(OrbMesh);
	OrbMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh.Succeeded())
		{
			OrbMesh->SetStaticMesh(SphereMesh.Object);
			OrbMesh->SetWorldScale3D(FVector(0.22f));
		}
	}

	GlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("GlowLight"));
	GlowLight->SetupAttachment(OrbMesh);
	GlowLight->SetLightColor(FLinearColor(0.4f, 0.85f, 1.f));
	GlowLight->SetIntensity(1200.f);
	GlowLight->SetAttenuationRadius(180.f);

	Steering = CreateDefaultSubobject<UMistspireSteeringComponent>(TEXT("Steering"));
	Steering->MaxSpeedCmPerSec = FollowSpeed;
	Steering->MaxForce = 1400.f;
	Steering->ArrivalRadiusCm = 40.f;
}

void AMistspireGuideSpirit::SetFollowTarget(AActor* InTarget)
{
	FollowTarget = InTarget;
}

void AMistspireGuideSpirit::SetBeaconDirection(const FVector& WorldDirection, float Intensity01)
{
	BeaconDirection = WorldDirection.GetSafeNormal();
	BeaconIntensity = FMath::Clamp(Intensity01, 0.f, 1.f);
}

void AMistspireGuideSpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FollowTarget.IsValid())
	{
		return;
	}

	OrbitAngle += DeltaTime * (1.2f + BeaconIntensity);
	const FVector OrbitOffset(
		FMath::Cos(OrbitAngle) * OrbitRadiusCm,
		FMath::Sin(OrbitAngle) * OrbitRadiusCm,
		30.f + 15.f * FMath::Sin(OrbitAngle * 2.f));

	const FVector ShoulderAnchor = FollowTarget->GetActorLocation()
		+ FollowTarget->GetActorRightVector() * -40.f
		+ FVector(0.f, 0.f, 140.f);

	const FVector BeaconHint = BeaconDirection * (80.f + 120.f * BeaconIntensity);
	const FVector Desired = ShoulderAnchor + OrbitOffset + BeaconHint;

	if (Steering)
	{
		// Steering behavior: arrive smoothly at the shoulder, then blend a push
		// toward the beacon so the orb leans into the next summit.
		Steering->SetTarget(ShoulderAnchor);
		const FVector SteeredVelocity = Steering->ComputeDesiredVelocity(
			GetActorLocation(), GetVelocity(), DeltaTime);

		const FVector SteeringVelocity = SteeredVelocity * SteeringBlend;
		const FVector HintVelocity = (Desired - GetActorLocation()).GetSafeNormal() * FollowSpeed * (1.f - SteeringBlend);
		SetActorLocation(GetActorLocation() + (SteeringVelocity + HintVelocity) * DeltaTime);
	}
	else
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), Desired, DeltaTime, FollowSpeed / 100.f));
	}

	if (GlowLight)
	{
		GlowLight->SetIntensity(800.f + 2400.f * BeaconIntensity);
	}
}