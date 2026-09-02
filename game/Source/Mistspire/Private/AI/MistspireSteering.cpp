#include "AI/MistspireSteering.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"

UMistspireSteeringComponent::UMistspireSteeringComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMistspireSteeringComponent::SetMode(EMistspireSteeringMode NewMode)
{
	Mode = NewMode;
}

void UMistspireSteeringComponent::SetTarget(const FVector& InTarget)
{
	TargetLocation = InTarget;
}

void UMistspireSteeringComponent::SetNeighbors(const TArray<AActor*>& InNeighbors)
{
	SeparationNeighbors.Reset();
	for (AActor* Neighbor : InNeighbors)
	{
		if (Neighbor)
		{
			SeparationNeighbors.Add(Neighbor);
		}
	}
}

bool UMistspireSteeringComponent::HasArrived(const FVector& Location) const
{
	return FVector::DistSquared(Location, TargetLocation) <= FMath::Square(ArrivalRadiusCm);
}

FVector UMistspireSteeringComponent::ComputeSeekForce(const FVector& Location, const FVector& CurrentVelocity) const
{
	const FVector Desired = (TargetLocation - Location).GetSafeNormal() * MaxSpeedCmPerSec;

	FVector Force = Desired - CurrentVelocity;
	if (Mode == EMistspireSteeringMode::Arrive)
	{
		const float Distance = FVector::Dist(Location, TargetLocation);
		const float Ramp = FMath::Clamp(Distance / FMath::Max(ArrivalRadiusCm, 1.f), 0.f, 1.f);
		Force = Desired * Ramp - CurrentVelocity;
	}
	else if (Mode == EMistspireSteeringMode::Flee)
	{
		Force = -Desired - CurrentVelocity;
	}
	return Force;
}

FVector UMistspireSteeringComponent::ComputeSeparationForce(const FVector& Location) const
{
	FVector Separation(0.f);
	for (const TWeakObjectPtr<AActor>& Neighbor : SeparationNeighbors)
	{
		if (!Neighbor.IsValid())
		{
			continue;
		}
		const FVector Delta = Location - Neighbor->GetActorLocation();
		const float Distance = Delta.Size();
		if (Distance > 0.f && Distance < SeparationRadiusCm)
		{
			Separation += Delta.GetSafeNormal() * (1.f - Distance / SeparationRadiusCm);
		}
	}
	return Separation * MaxSpeedCmPerSec * SeparationStrength;
}

FVector UMistspireSteeringComponent::ComputeDesiredVelocity(const FVector& Location, const FVector& CurrentVelocity, float DeltaTime) const
{
	if (Mode == EMistspireSteeringMode::Wander)
	{
		WanderAngle += DeltaTime * 2.5f;
		const FVector Forward = CurrentVelocity.SizeSquared() > 1.f
			? CurrentVelocity.GetSafeNormal()
			: FVector(1.f, 0.f, 0.35f).GetSafeNormal();
		const FVector Side = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
		const FVector WanderOffset = (Forward * FMath::Cos(WanderAngle) + Side * FMath::Sin(WanderAngle)) * WanderRadiusCm;
		WanderTarget = Location + WanderOffset;
	}

	FVector Force = ComputeSeekForce(Location, CurrentVelocity);
	if (Mode == EMistspireSteeringMode::Wander)
	{
		Force = (WanderTarget - Location).GetSafeNormal() * MaxSpeedCmPerSec - CurrentVelocity;
	}

	Force += ComputeSeparationForce(Location);

	// Truncate to max force, then clamp speed.
	if (Force.SizeSquared() > FMath::Square(MaxForce))
	{
		Force = Force.GetSafeNormal() * MaxForce;
	}

	FVector Velocity = CurrentVelocity + Force * DeltaTime;
	if (Velocity.SizeSquared() > FMath::Square(MaxSpeedCmPerSec))
	{
		Velocity = Velocity.GetSafeNormal() * MaxSpeedCmPerSec;
	}
	return Velocity;
}