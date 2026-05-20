#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MistspireVRPawn.generated.h"

class UCameraComponent;
class UCapsuleComponent;

UCLASS()
class MISTSPIRE_API AMistspireVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AMistspireVRPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ApplySmoothLocomotion(FVector2D MoveInput, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ApplyTeleport(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void TeleportForward(float DistanceCm = 800.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void StartClimb();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void StopClimb();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void FireGrapple(FVector WorldTarget);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ToggleGlider(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void TryJump();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UCameraComponent> VRCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float DefaultLocomotionSpeedCmPerSec = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float LocomotionSpeedCmPerSec = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float TurnRateDegPerSec = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float JumpImpulseCmPerSec = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float TeleportForwardCm = 800.f;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bIsClimbing = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bGliderActive = false;

private:
	void PollXRInput();
	void UpdateAltitudeTracking();
	void ApplyVerticalVelocity(float DeltaCm);

	FVector2D CachedMoveInput;
	float CachedTurnInput = 0.f;
	float VerticalVelocityCmPerSec = 0.f;
	bool bMenuPressedLast = false;
	bool bJumpPressedLast = false;
};
