#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MistspireSteering.generated.h"

UENUM(BlueprintType)
enum class EMistspireSteeringMode : uint8
{
	Seek,
	Arrive,
	Flee,
	Wander,
};

/** Free-flight steering behaviors for floating actors (Godot Steering equivalent). */
UCLASS(ClassGroup = (Mistspire), meta = (BlueprintSpawnableComponent))
class MISTSPIRE_API UMistspireSteeringComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMistspireSteeringComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	EMistspireSteeringMode Mode = EMistspireSteeringMode::Arrive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	float MaxSpeedCmPerSec = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	float MaxForce = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	float ArrivalRadiusCm = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	float WanderRadiusCm = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	float SeparationRadiusCm = 260.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|AI|Steering")
	float SeparationStrength = 1.2f;

	/** Actors pushed away by separation (e.g. other ghosts, the player). */
	UPROPERTY(EditAnywhere, Category = "Mistspire|AI|Steering")
	TArray<TWeakObjectPtr<AActor>> SeparationNeighbors;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Steering")
	void SetMode(EMistspireSteeringMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Steering")
	void SetTarget(const FVector& InTarget);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Steering")
	void SetNeighbors(const TArray<AActor*>& InNeighbors);

	/** Returns a steering-constrained velocity to apply to the owning actor. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|AI|Steering")
	FVector ComputeDesiredVelocity(const FVector& Location, const FVector& CurrentVelocity, float DeltaTime) const;

	/** True when an arriving agent is within ArrivalRadiusCm of the target. */
	UFUNCTION(BlueprintPure, Category = "Mistspire|AI|Steering")
	bool HasArrived(const FVector& Location) const;

private:
	FVector ComputeSeekForce(const FVector& Location, const FVector& CurrentVelocity) const;
	FVector ComputeSeparationForce(const FVector& Location) const;

	mutable FVector WanderTarget = FVector::ZeroVector;
	mutable float WanderAngle = 0.f;
};