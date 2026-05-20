#include "MistspireVRPawn.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireSummitRegistry.h"
#include "MistspireXRActionSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"

AMistspireVRPawn::AMistspireVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(34.f, 88.f);
	SetRootComponent(Capsule);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(Capsule);
	VRCamera->bUsePawnControlRotation = false;

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
	PollXRInput();
	ApplySmoothLocomotion(CachedMoveInput, DeltaTime);
	ApplyVerticalVelocity(VerticalVelocityCmPerSec * DeltaTime);
	VerticalVelocityCmPerSec = FMath::FInterpTo(VerticalVelocityCmPerSec, 0.f, DeltaTime, 4.f);
	UpdateAltitudeTracking();
}

void AMistspireVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMistspireVRPawn::PollXRInput()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UMistspireXRActionSubsystem* XR = World->GetSubsystem<UMistspireXRActionSubsystem>();
	if (!XR)
	{
		return;
	}

	const FMistspireXRInputState& State = XR->GetInputState();
	CachedMoveInput = FVector2D(State.MoveX, State.MoveY);
	CachedTurnInput = State.Turn;

	if (State.bClimbPressed && !bIsClimbing)
	{
		StartClimb();
	}
	else if (!State.bClimbPressed && bIsClimbing)
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
	if (MoveInput.IsNearlyZero())
	{
		return;
	}

	const FRotator YawRot(0.f, GetActorRotation().Yaw + CachedTurnInput * TurnRateDegPerSec * DeltaTime, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	const FVector Delta = (Forward * MoveInput.Y + Right * MoveInput.X) * LocomotionSpeedCmPerSec * DeltaTime;

	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);
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
}

void AMistspireVRPawn::StopClimb()
{
	bIsClimbing = false;
	LocomotionSpeedCmPerSec = bGliderActive ? DefaultLocomotionSpeedCmPerSec * 1.5f : DefaultLocomotionSpeedCmPerSec;
}

void AMistspireVRPawn::FireGrapple(FVector WorldTarget)
{
	const FVector ToTarget = WorldTarget - GetActorLocation();
	const float Pull = FMath::Min(ToTarget.Size(), 800.f);
	AddActorWorldOffset(ToTarget.GetSafeNormal() * Pull, true);
}

void AMistspireVRPawn::ToggleGlider(bool bEnable)
{
	bGliderActive = bEnable;
	LocomotionSpeedCmPerSec = bGliderActive ? DefaultLocomotionSpeedCmPerSec * 1.5f : DefaultLocomotionSpeedCmPerSec;
}

void AMistspireVRPawn::TryJump()
{
	VerticalVelocityCmPerSec = JumpImpulseCmPerSec;
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
