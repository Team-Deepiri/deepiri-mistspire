#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MistspireVRPawn.generated.h"

class UCameraComponent;
class UCapsuleComponent;
class UMotionControllerComponent;
class USkeletalMeshComponent;

UCLASS()
class MISTSPIRE_API AMistspireVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AMistspireVRPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ApplySmoothLocomotion(FVector2D MoveInput, float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplySmoothLocomotion(FVector Delta);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ApplyTeleport(const FVector& TargetLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyTeleport(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void TeleportForward(float DistanceCm = 800.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void StartClimb();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartClimb();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void StopClimb();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopClimb();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void FireGrapple(FVector WorldTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FireGrapple(FVector WorldTarget);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ToggleGlider(bool bEnable);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ToggleGlider(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void TryJump();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TryJump();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UCameraComponent> VRCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UMotionControllerComponent> LeftHandController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UMotionControllerComponent> RightHandController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<USkeletalMeshComponent> LeftHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<USkeletalMeshComponent> RightHandMesh;

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

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bIsClimbing = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Traversal")
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
