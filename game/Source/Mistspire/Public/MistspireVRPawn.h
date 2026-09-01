#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MistspireVRPawn.generated.h"
class UCapsuleComponent;
class UMotionControllerComponent;
class USkeletalMeshComponent;
class UTextRenderComponent;
class UAudioComponent;
class UPostProcessComponent;
class UCableComponent;

UCLASS()
class MISTSPIRE_API AMistspireVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AMistspireVRPawn();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void NotifyControllerChanged() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** True when playing without a headset (editor PIE, packaged -nonvr, or no HMD). */
	UFUNCTION(BlueprintPure, Category = "Mistspire|NonVR")
	bool IsNonVRMode() const { return bNonVRMode; }

	/** Camera-forward point used for non-VR interaction traces. */
	UFUNCTION(BlueprintPure, Category = "Mistspire|NonVR")
	FVector GetInteractionTraceStart() const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|NonVR")
	FVector GetInteractionTraceEnd(float MaxDistanceCm = 400.f) const;

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

	void UpdateClimbingMovement(float DeltaTime);
	void UpdateGlidingMovement(float DeltaTime);
	void UpdateImmersiveAudio(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void SetHandGrip(bool bIsLeft, bool bGripped);

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Traversal")
	FVector LeftHandAnchor;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Traversal")
	FVector RightHandAnchor;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bLeftHandGripped = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bRightHandGripped = false;

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

	UFUNCTION(BlueprintPure, Category = "Mistspire|VR")
	FVector GetLeftHandWorldLocation() const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|VR")
	FVector GetRightHandWorldLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Survival")
	void ApplyShelterRefill(float OxygenPerSec, float StaminaPerSec, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Traversal")
	void ApplyWindCrystalBoost(float DurationSeconds, float StaminaRestore);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Survival")
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Survival")
	float GetOxygenPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|UI")
	void ShowNotification(const FString& Message, float Duration = 3.f);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Survival")
	float GetAtmosphericPressure() const;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<USkeletalMeshComponent> VisualLeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<USkeletalMeshComponent> VisualRightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<USkeletalMeshComponent> FullBodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Interaction")
	TObjectPtr<USceneComponent> LeftHolster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Interaction")
	TObjectPtr<USceneComponent> RightHolster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UCableComponent> GrappleCable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UStaticMeshComponent> GliderMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|VR")
	TObjectPtr<UPostProcessComponent> ComfortVignette;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Audio")
	TObjectPtr<UAudioComponent> WindAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Audio")
	TObjectPtr<UAudioComponent> ExertionAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|UI")
	TObjectPtr<UTextRenderComponent> AltimeterText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|UI")
	TObjectPtr<UTextRenderComponent> StaminaWristText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|UI")
	TObjectPtr<UTextRenderComponent> OxygenWristText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|UI")
	TObjectPtr<UTextRenderComponent> BeaconWristText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|UI")
	TObjectPtr<UTextRenderComponent> BiomeWristText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|UI")
	TObjectPtr<UTextRenderComponent> NotificationText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Audio")
	TObjectPtr<UAudioComponent> HeartbeatAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Audio")
	TObjectPtr<UAudioComponent> SummitChimeAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float GrappleTraceDistanceCm = 8000.f;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|NonVR")
	float NonVREyeHeightCm = 64.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Traversal")
	float SprintSpeedCmPerSec = 700.f;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|NonVR")
	bool bNonVRMode = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bIsClimbing = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bGliderActive = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Traversal")
	bool bGrappleActive = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Traversal")
	FVector GrappleAnchorPoint;

	// Stamina & Immersion Stats
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Survival")
	float CurrentStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float StaminaDrainRateClimbing = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float StaminaDrainRateGliding = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float StaminaRecoveryRate = 12.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Survival")
	bool bIsExhausted = false;

	// High Altitude Survival
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Survival")
	float CurrentOxygen = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float MaxOxygen = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float OxygenDrainRateBase = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Survival")
	float OxygenRecoveryRate = 20.f;

	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Survival")
	float CurrentAtmosphericPressure = 1.0f;

private:
	void ConfigureNonVRMode();
	void ApplyNonVRPlayerControllerSettings();
	void PollNonVRInput();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void OnClimbPressed();
	void OnClimbReleased();
	void OnSprintPressed();
	void OnSprintReleased();
	void OnGrapplePressed();
	void OnGliderPressed();
	void OnTeleportPressed();
	void OnInteractPressed();
	void PollXRInput();
	void UpdateAltitudeTracking();
	void ApplyVerticalVelocity(float DeltaCm);
	void UpdateStamina(float DeltaTime);
	void UpdateOxygen(float DeltaTime);
	void UpdateAtmosphericEffects(float DeltaTime);
	void UpdateWristHUD();
	void TryGrappleShot();
	void TryMantle(float DeltaTime);
	void UpdateBeaconPulseHaptics();
	UFUNCTION()
	void HandleSummitReached(FName SummitId);

	FVector2D CachedMoveInput;
	float NonVRMoveForward = 0.f;
	float NonVRMoveRight = 0.f;
	float CachedTurnInput = 0.f;
	bool bNonVRClimbHeld = false;
	bool bNonVRSprintHeld = false;
	float VerticalVelocityCmPerSec = 0.f;
	bool bMenuPressedLast = false;
	bool bMenuHeld = false;
	float MenuHoldTimer = 0.f;
	bool bJumpPressedLast = false;
	bool bGrapplePressedLast = false;
	bool bGliderPressedLast = false;
	FVector GliderVelocity = FVector::ZeroVector;
	float GliderBoostTimeRemaining = 0.f;
	float GliderBoostMultiplier = 1.65f;
	bool bGrappleHeld = false;
	float NotificationTimer = 0.f;
};
