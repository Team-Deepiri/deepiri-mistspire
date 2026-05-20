#include "MistspireVRPawn.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireSummitRegistry.h"
#include "MistspireXRActionSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AMistspireVRPawn::AMistspireVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(34.f, 88.f);
	Capsule->SetIsReplicated(true);
	SetRootComponent(Capsule);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(Capsule);
	VRCamera->bUsePawnControlRotation = false;

	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(Capsule);
	LeftHandController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;

	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftHandController);

	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(Capsule);
	RightHandController->MotionSource = FXRMotionControllerBase::RightHandSourceId;

	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHandController);

	LocomotionSpeedCmPerSec = DefaultLocomotionSpeedCmPerSec;
}

void AMistspireVRPawn::BeginPlay()
{
	Super::BeginPlay();
	LocomotionSpeedCmPerSec = DefaultLocomotionSpeedCmPerSec;
}

void AMistspireVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsLocallyControlled())
	{
		PollXRInput();
		ApplySmoothLocomotion(CachedMoveInput, DeltaTime);
		ApplyVerticalVelocity(VerticalVelocityCmPerSec * DeltaTime);
		VerticalVelocityCmPerSec = FMath::FInterpTo(VerticalVelocityCmPerSec, 0.f, DeltaTime, 4.f);
		UpdateAltitudeTracking();
	}
}

void AMistspireVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMistspireVRPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const
{
	Super::GetLifetimeReplicatedProps(OutReplicatedProps);

	DOREPLIFETIME(AMistspireVRPawn, bIsClimbing);
	DOREPLIFETIME(AMistspireVRPawn, bGliderActive);
}

void AMistspireVRPawn::PollXRInput()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UMistspireXRActionSubsystem* XR = World->GetSubsystem<UMistspireXRActionSubsystem>();
	if (!XR) return;

	const FMistspireXRInputState& State = XR->GetInputState();
	CachedMoveInput = FVector2D(State.MoveX, State.MoveY);
	CachedTurnInput = State.Turn;

	// Per-hand Grip/Climb logic
	SetHandGrip(true, State.bGrabLeft || State.bClimbLeft);
	SetHandGrip(false, State.bGrabRight || State.bClimbRight);

	if ((bLeftHandGripped || bRightHandGripped) && !bIsClimbing)
	{
		StartClimb();
	}
	else if (!bLeftHandGripped && !bRightHandGripped && bIsClimbing)
	{
		StopClimb();
	}

	if (State.bMenuPressed && !bMenuPressedLast)
	{
		TeleportForward(TeleportForwardCm);
	}
	bMenuPressedLast = State.bMenuPressed;

	if (State.bJumpPressed && !bJumpPressedLast)
	{
		TryJump();
	}
	bJumpPressedLast = State.bJumpPressed;
}

void AMistspireVRPawn::ApplySmoothLocomotion(FVector2D MoveInput, float DeltaTime)
{
	if (MoveInput.IsNearlyZero() && FMath::IsNearlyZero(CachedTurnInput))
	{
		return;
	}

	const FRotator YawRot(0.f, GetActorRotation().Yaw + CachedTurnInput * TurnRateDegPerSec * DeltaTime, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	const FVector Delta = (Forward * MoveInput.Y + Right * MoveInput.X) * LocomotionSpeedCmPerSec * DeltaTime;

	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);
	
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplySmoothLocomotion(Delta);
	}
}

bool AMistspireVRPawn::Server_ApplySmoothLocomotion_Validate(FVector Delta) { return true; }
void AMistspireVRPawn::Server_ApplySmoothLocomotion_Implementation(FVector Delta)
{
	AddActorWorldOffset(Delta, true);
}

void AMistspireVRPawn::SetHandGrip(bool bIsLeft, bool bGripped)
{
	if (bIsLeft)
	{
		if (bGripped && !bLeftHandGripped)
		{
			LeftHandAnchor = LeftHandController->GetRelativeLocation();
		}
		bLeftHandGripped = bGripped;
	}
	else
	{
		if (bGripped && !bRightHandGripped)
		{
			RightHandAnchor = RightHandController->GetRelativeLocation();
		}
		bRightHandGripped = bGripped;
	}
}

void AMistspireVRPawn::UpdateClimbingMovement(float DeltaTime)
{
	FVector TotalDelta = FVector::ZeroVector;
	int32 ActiveHands = 0;

	if (bLeftHandGripped)
	{
		TotalDelta += (LeftHandAnchor - LeftHandController->GetRelativeLocation());
		ActiveHands++;
	}
	if (bRightHandGripped)
	{
		TotalDelta += (RightHandAnchor - RightHandController->GetRelativeLocation());
		ActiveHands++;
	}

	if (ActiveHands > 0)
	{
		FVector AverageDelta = TotalDelta / (float)ActiveHands;
		// Convert local delta to world delta based on pawn rotation
		FVector WorldDelta = GetActorRotation().RotateVector(AverageDelta);
		
		FHitResult Hit;
		AddActorWorldOffset(WorldDelta, true, &Hit);

		// Synchronize movement to server
		if (GetLocalRole() < ROLE_Authority)
		{
			Server_ApplySmoothLocomotion(WorldDelta);
		}
	}
}

void AMistspireVRPawn::UpdateGlidingMovement(float DeltaTime)
{
	// 1. Gravity
	GliderVelocity += FVector(0, 0, -600.f) * DeltaTime;

	// 2. Drag
	float Speed = GliderVelocity.Size();
	FVector Drag = -GliderVelocity.GetSafeNormal() * (Speed * 0.05f);
	GliderVelocity += Drag * DeltaTime;

	// 3. Lift & Steering
	FVector GazeDirection = VRCamera->GetForwardVector();
	
	// Dive/Lift based on pitch
	float Pitch = GazeDirection.Z; // -1 to 1
	GliderVelocity += GazeDirection * (Pitch < 0 ? -Pitch * 500.f : -Pitch * 200.f) * DeltaTime;

	// Rotate velocity toward gaze
	GliderVelocity = FMath::VInterpTo(GliderVelocity, GazeDirection * Speed, DeltaTime, 2.0f);

	// 4. Apply movement
	FHitResult Hit;
	AddActorWorldOffset(GliderVelocity * DeltaTime, true, &Hit);

	// Synchronize movement to server
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplySmoothLocomotion(GliderVelocity * DeltaTime);
	}
}

void AMistspireVRPawn::ApplyVerticalVelocity(float DeltaCm)
{
	if (!FMath::IsNearlyZero(DeltaCm))
	{
		AddActorWorldOffset(FVector(0.f, 0.f, DeltaCm), true);
	}
}

void AMistspireVRPawn::ApplyTeleport(const FVector& TargetLocation)
{
	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplyTeleport(TargetLocation);
	}
}

bool AMistspireVRPawn::Server_ApplyTeleport_Validate(const FVector& TargetLocation) { return true; }
void AMistspireVRPawn::Server_ApplyTeleport_Implementation(const FVector& TargetLocation)
{
	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AMistspireVRPawn::TeleportForward(float DistanceCm)
{
	const FVector Target = GetActorLocation() + VRCamera->GetForwardVector() * DistanceCm;
	ApplyTeleport(Target);
}

void AMistspireVRPawn::StartClimb()
{
	bIsClimbing = true;
	LocomotionSpeedCmPerSec = DefaultLocomotionSpeedCmPerSec * 0.55f;
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_StartClimb();
	}
}

bool AMistspireVRPawn::Server_StartClimb_Validate() { return true; }
void AMistspireVRPawn::Server_StartClimb_Implementation() { bIsClimbing = true; }

void AMistspireVRPawn::StopClimb()
{
	bIsClimbing = false;
	LocomotionSpeedCmPerSec = bGliderActive ? DefaultLocomotionSpeedCmPerSec * 1.5f : DefaultLocomotionSpeedCmPerSec;
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_StopClimb();
	}
}

bool AMistspireVRPawn::Server_StopClimb_Validate() { return true; }
void AMistspireVRPawn::Server_StopClimb_Implementation() { bIsClimbing = false; }

void AMistspireVRPawn::FireGrapple(FVector WorldTarget)
{
	const FVector ToTarget = WorldTarget - GetActorLocation();
	const float Pull = FMath::Min(ToTarget.Size(), 800.f);
	const FVector Delta = ToTarget.GetSafeNormal() * Pull;
	AddActorWorldOffset(Delta, true);
	
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_FireGrapple(WorldTarget);
	}
}

bool AMistspireVRPawn::Server_FireGrapple_Validate(FVector WorldTarget) { return true; }
void AMistspireVRPawn::Server_FireGrapple_Implementation(FVector WorldTarget)
{
	FireGrapple(WorldTarget);
}

void AMistspireVRPawn::ToggleGlider(bool bEnable)
{
	bGliderActive = bEnable;
	LocomotionSpeedCmPerSec = bGliderActive ? DefaultLocomotionSpeedCmPerSec * 1.5f : DefaultLocomotionSpeedCmPerSec;
	
	if (bGliderActive)
	{
		GliderVelocity = GetVelocity();
		if (GliderVelocity.IsNearlyZero())
		{
			GliderVelocity = GetActorForwardVector() * LocomotionSpeedCmPerSec;
		}
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ToggleGlider(bEnable);
	}
}

bool AMistspireVRPawn::Server_ToggleGlider_Validate(bool bEnable) { return true; }
void AMistspireVRPawn::Server_ToggleGlider_Implementation(bool bEnable) { bGliderActive = bEnable; }

void AMistspireVRPawn::TryJump()
{
	VerticalVelocityCmPerSec = JumpImpulseCmPerSec;
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_TryJump();
	}
}

bool AMistspireVRPawn::Server_TryJump_Validate() { return true; }
void AMistspireVRPawn::Server_TryJump_Implementation()
{
	TryJump();
}

void AMistspireVRPawn::UpdateAltitudeTracking()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
	{
		Alt->UpdateAltitudeFromWorldLocation(GetActorLocation());
	}

	if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
	{
		static const FName SummitIds[] = {
			TEXT("summit_valley_gate"),
			TEXT("summit_mesa_crown"),
			TEXT("summit_cloud_garden"),
			TEXT("summit_obelisk_prime"),
			TEXT("summit_orbital_needle"),
		};
		for (FName Id : SummitIds)
		{
			Registry->TryReachSummit(Id, GetActorLocation());
		}
	}
}
