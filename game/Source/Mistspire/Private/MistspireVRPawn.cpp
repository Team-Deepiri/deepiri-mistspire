#include "MistspireVRPawn.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireSummitRegistry.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireBeaconSubsystem.h"
#include "MistspirePlayerState.h"
#include "MistspireProgressSubsystem.h"
#include "MistspireAmbienceSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireInteriorSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "CableComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

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

	ComfortVignette = CreateDefaultSubobject<UPostProcessComponent>(TEXT("ComfortVignette"));
	ComfortVignette->SetupAttachment(VRCamera);

	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(Capsule);
	LeftHandController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;

	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftHandController);
	LeftHandMesh->SetHiddenInGame(true);

	VisualLeftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VisualLeftHand"));
	VisualLeftHand->SetupAttachment(Capsule);

	AltimeterText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AltimeterText"));
	AltimeterText->SetupAttachment(VisualLeftHand);
	AltimeterText->SetRelativeLocation(FVector(0.f, 0.f, 12.f));
	AltimeterText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	AltimeterText->SetHorizontalAlignment(EHTA_Center);
	AltimeterText->SetWorldSize(4.f);

	StaminaWristText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StaminaWristText"));
	StaminaWristText->SetupAttachment(VisualLeftHand);
	StaminaWristText->SetRelativeLocation(FVector(0.f, 8.f, 6.f));
	StaminaWristText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	StaminaWristText->SetHorizontalAlignment(EHTA_Center);
	StaminaWristText->SetWorldSize(2.5f);

	OxygenWristText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OxygenWristText"));
	OxygenWristText->SetupAttachment(VisualLeftHand);
	OxygenWristText->SetRelativeLocation(FVector(0.f, 8.f, 2.f));
	OxygenWristText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	OxygenWristText->SetHorizontalAlignment(EHTA_Center);
	OxygenWristText->SetWorldSize(2.5f);

	BeaconWristText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("BeaconWristText"));
	BeaconWristText->SetupAttachment(VisualLeftHand);
	BeaconWristText->SetRelativeLocation(FVector(0.f, 8.f, -2.f));
	BeaconWristText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	BeaconWristText->SetHorizontalAlignment(EHTA_Center);
	BeaconWristText->SetWorldSize(2.2f);

	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(Capsule);
	RightHandController->MotionSource = FXRMotionControllerBase::RightHandSourceId;

	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHandController);
	RightHandMesh->SetHiddenInGame(true);

	VisualRightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VisualRightHand"));
	VisualRightHand->SetupAttachment(Capsule);

	FullBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FullBodyMesh"));
	FullBodyMesh->SetupAttachment(Capsule);
	FullBodyMesh->SetOwnerNoSee(true); // Player shouldn't see their own head/neck inside camera

	LeftHolster = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHolster"));
	LeftHolster->SetupAttachment(Capsule);

	RightHolster = CreateDefaultSubobject<USceneComponent>(TEXT("RightHolster"));
	RightHolster->SetupAttachment(Capsule);

	GrappleCable = CreateDefaultSubobject<UCableComponent>(TEXT("GrappleCable"));
	GrappleCable->SetupAttachment(VisualRightHand);
	GrappleCable->SetHiddenInGame(true);
	GrappleCable->CableLength = 0.f;

	GliderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GliderMesh"));
	GliderMesh->SetupAttachment(Capsule);
	GliderMesh->SetHiddenInGame(true);

	WindAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("WindAudio"));
	WindAudio->SetupAttachment(VRCamera);

	ExertionAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ExertionAudio"));
	ExertionAudio->SetupAttachment(VRCamera);

	HeartbeatAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HeartbeatAudio"));
	HeartbeatAudio->SetupAttachment(VRCamera);
	HeartbeatAudio->bAutoActivate = false;

	SummitChimeAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SummitChimeAudio"));
	SummitChimeAudio->SetupAttachment(VRCamera);
	SummitChimeAudio->bAutoActivate = false;

	LocomotionSpeedCmPerSec = DefaultLocomotionSpeedCmPerSec;
}

void AMistspireVRPawn::BeginPlay()
{
	Super::BeginPlay();
	LocomotionSpeedCmPerSec = DefaultLocomotionSpeedCmPerSec;

	if (Capsule)
	{
		Capsule->SetGenerateOverlapEvents(true);
	}

	if (UWorld* World = GetWorld())
	{
		if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
		{
			SummitReachedHandle = Registry->OnSummitReached.AddUObject(this, &AMistspireVRPawn::HandleSummitReached);
		}
	}
}

void AMistspireVRPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
		{
			Registry->OnSummitReached.Remove(SummitReachedHandle);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AMistspireVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsLocallyControlled())
	{
		UWorld* World = GetWorld();
		PollXRInput();
		
		if (bIsClimbing)
		{
			UpdateClimbingMovement(DeltaTime);
		}
		else if (bGliderActive)
		{
			UpdateGlidingMovement(DeltaTime);
		}
		else
		{
			ApplySmoothLocomotion(CachedMoveInput, DeltaTime);
			ApplyVerticalVelocity(VerticalVelocityCmPerSec * DeltaTime);
			VerticalVelocityCmPerSec = FMath::FInterpTo(VerticalVelocityCmPerSec, 0.f, DeltaTime, 4.f);
		}
		
		UpdateAltitudeTracking();
		UpdateStamina(DeltaTime);
		UpdateOxygen(DeltaTime);
		UpdateAtmosphericEffects(DeltaTime);
		UpdateImmersiveAudio(DeltaTime);
		UpdateWristHUD();
		TryMantle(DeltaTime);
		UpdateBeaconPulseHaptics();

		if (GliderBoostTimeRemaining > 0.f)
		{
			GliderBoostTimeRemaining -= DeltaTime;
		}

		if (UMistspireZoneSubsystem* Zone = World->GetSubsystem<UMistspireZoneSubsystem>())
		{
			if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
			{
				Zone->UpdateZoneFromAltitude(Alt->GetCurrentAltitudeCm());
			}
		}

		if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
		{
			Atlas->UpdateDistrictFromPlayerLocation(GetActorLocation());
		}

		// Comfort Vignette based on rotation, speed, exhaustion, and hypoxia
		if (ComfortVignette)
		{
			float TurnFactor = FMath::Abs(CachedTurnInput);
			float SpeedFactor = GliderVelocity.Size() / 4000.f;
			float ExhaustionFactor = bIsExhausted ? 0.4f : (1.0f - (CurrentStamina / MaxStamina)) * 0.3f;
			float HypoxiaFactor = (1.0f - (CurrentOxygen / MaxOxygen)) * 0.8f;
			float Intensity = FMath::Max(FMath::Max(FMath::Max(TurnFactor, SpeedFactor * 0.5f), ExhaustionFactor), HypoxiaFactor);
			ComfortVignette->BlendWeight = FMath::FInterpTo(ComfortVignette->BlendWeight, Intensity, DeltaTime, 5.f);
		}

		// Grapple Physics Pull (hold trigger to reel faster)
		if (bGrappleActive)
		{
			float PullStrength = 1200.f;
			if (UMistspireXRActionSubsystem* XRInput = World->GetSubsystem<UMistspireXRActionSubsystem>())
			{
				if (XRInput->GetInputState().bGrapplePressed)
				{
					PullStrength = 2200.f;
				}
			}

			FVector ToAnchor = GrappleAnchorPoint - GetActorLocation();
			float Dist = ToAnchor.Size();
			if (Dist > 100.f)
			{
				FVector PullForce = ToAnchor.GetSafeNormal() * PullStrength * DeltaTime;
				AddActorWorldOffset(PullForce, true);
				
				if (GetLocalRole() < ROLE_Authority)
				{
					Server_ApplySmoothLocomotion(PullForce);
				}
			}
			else
			{
				bGrappleActive = false;
				GrappleCable->SetHiddenInGame(true);
			}
		}

		// Physical Hand Collisions
		auto UpdateHandPhysics = [&](USkeletalMeshComponent* VisualHand, UMotionControllerComponent* Controller)
		{
			if (!VisualHand || !Controller) return;
			
			FVector TargetLoc = Controller->GetComponentLocation();
			FRotator TargetRot = Controller->GetComponentRotation();
			
			FHitResult Hit;
			VisualHand->SetWorldLocationAndRotation(TargetLoc, TargetRot, true, &Hit);
			
			// If we hit something, triggered haptics to feel the "bump"
			if (Hit.bBlockingHit)
			{
				if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
				{
					bool bIsLeft = (VisualHand == VisualLeftHand);
					XR->TriggerHapticVibration(bIsLeft, 0.3f, 0.05f, 100.f);
				}
			}
		};

		UpdateHandPhysics(VisualLeftHand, LeftHandController);
		UpdateHandPhysics(VisualRightHand, RightHandController);

		// VRIK Presence & Holster Tracking
		if (FullBodyMesh && VRCamera)
		{
			// Position body mesh under camera but keep at ground level
			FVector CameraLoc = VRCamera->GetRelativeLocation();
			FVector BodyLoc = FVector(CameraLoc.X, CameraLoc.Y, -Capsule->GetScaledCapsuleHalfHeight());
			FullBodyMesh->SetRelativeLocation(BodyLoc);

			// Rotate body with head yaw
			FRotator HeadRot = VRCamera->GetRelativeRotation();
			FRotator BodyRot(0.f, HeadRot.Yaw, 0.f);
			FullBodyMesh->SetRelativeRotation(BodyRot);

			// Position holsters at waist level
			FVector WaistOffset = BodyLoc + FVector(0.f, 0.f, 100.f); // ~1m up from ground
			if (LeftHolster)
			{
				FVector LPos = WaistOffset + BodyRot.RotateVector(FVector(0.f, -25.f, 0.f));
				LeftHolster->SetRelativeLocation(LPos);
				LeftHolster->SetRelativeRotation(BodyRot);
			}
			if (RightHolster)
			{
				FVector RPos = WaistOffset + BodyRot.RotateVector(FVector(0.f, 25.f, 0.f));
				RightHolster->SetRelativeLocation(RPos);
				RightHolster->SetRelativeRotation(BodyRot);
			}
		}

		// Weather-Specific Haptics (Static Electricity, Storm Turbulence)
		if (World)
		{
			if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
			{
				EMistspireWeatherType Weather = Env->GetCurrentWeather();
				if (Weather == EMistspireWeatherType::ElectricTurmoil)
				{
					// Random static "pops" in the hands
					if (FMath::FRand() < 0.05f)
					{
						if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
						{
							XR->TriggerHapticVibration(FMath::RandBool(), 0.1f, 0.02f, 300.f);
						}
					}
				}
			}
		}

		// Adrenaline & Heartbeat Haptics (Scale with Exhaustion, Hypoxia, and Panic)
		static float HeartbeatTimer = 0.f;
		HeartbeatTimer += DeltaTime;
		
		float SpeedFactor = GliderVelocity.Size() / 3000.f;
		float AltitudeFactor = GetActorLocation().Z / 500000.f;
		float ExhaustionFactor = (1.0f - (CurrentStamina / MaxStamina));
		float HypoxiaFactor = (1.0f - (CurrentOxygen / MaxOxygen));
		
		// Panic Factor: High vertical velocity without glider/climb
		float PanicFactor = (!bIsClimbing && !bGliderActive && VerticalVelocityCmPerSec < -500.f) ? FMath::Clamp(-VerticalVelocityCmPerSec / 2000.f, 0.f, 1.0f) : 0.f;

		float Adrenaline = FMath::Clamp(SpeedFactor + AltitudeFactor + ExhaustionFactor + HypoxiaFactor + PanicFactor, 0.f, 1.5f);
		
		if (Adrenaline > 0.3f)
		{
			float PulseRate = FMath::Lerp(1.5f, 0.25f, FMath::Min(Adrenaline, 1.0f)); 
			if (HeartbeatTimer >= PulseRate)
			{
				HeartbeatTimer = 0.f;
				if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
				{
					float Strength = 0.15f * Adrenaline;
					XR->TriggerHapticVibration(true, Strength, 0.08f, 40.f);
					XR->TriggerHapticVibration(false, Strength, 0.08f, 40.f);
				}
			}
		}

		if (AltimeterText && World)
		{
			if (UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>())
			{
				const float Meters = Alt->GetCurrentAltitudeCm() / 100.f;
				AltimeterText->SetText(FText::Format(NSLOCTEXT("Mistspire", "AltFormat", "{0}m"), FText::AsNumber(FMath::RoundToInt(Meters))));
			}
		}

		if (HeartbeatAudio)
		{
			const float HeartVol = FMath::Clamp(Adrenaline * 0.35f, 0.f, 0.5f);
			if (HeartVol > 0.08f)
			{
				if (!HeartbeatAudio->IsPlaying()) HeartbeatAudio->Play();
				HeartbeatAudio->SetVolumeMultiplier(HeartVol);
				HeartbeatAudio->SetPitchMultiplier(FMath::Lerp(0.85f, 1.4f, Adrenaline));
			}
			else if (HeartbeatAudio->IsPlaying())
			{
				HeartbeatAudio->Stop();
			}
		}
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
	DOREPLIFETIME(AMistspireVRPawn, bGrappleActive);
	DOREPLIFETIME(AMistspireVRPawn, GrappleAnchorPoint);
	DOREPLIFETIME(AMistspireVRPawn, CurrentStamina);
	DOREPLIFETIME(AMistspireVRPawn, CurrentOxygen);
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

	if (State.bGrapplePressed && !bGrapplePressedLast)
	{
		TryGrappleShot();
	}
	bGrapplePressedLast = State.bGrapplePressed;

	if (State.bGliderPressed && !bGliderPressedLast)
	{
		ToggleGlider(!bGliderActive);
	}
	bGliderPressedLast = State.bGliderPressed;
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
		FVector WorldDelta = GetActorRotation().RotateVector(AverageDelta);
		
		FHitResult Hit;
		AddActorWorldOffset(WorldDelta, true, &Hit);

		// Haptic Feedback for climbing
		if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			float GripForce = WorldDelta.Size() / (DeltaTime * 100.f);
			
			// Constant "Surface Grain" feel while moving hands
			float GrainStrength = FMath::Clamp(GripForce * 0.05f, 0.02f, 0.15f);
			
			if (bLeftHandGripped) XR->TriggerHapticVibration(true, FMath::Min(GripForce * 0.1f + GrainStrength, 0.5f), 0.05f, 160.f);
			if (bRightHandGripped) XR->TriggerHapticVibration(false, FMath::Min(GripForce * 0.1f + GrainStrength, 0.5f), 0.05f, 160.f);
		}

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

	// 2. Environment (Wind) + wind-crystal boost
	const float BoostMult = (GliderBoostTimeRemaining > 0.f) ? GliderBoostMultiplier : 1.f;
	FVector Wind = FVector::ZeroVector;
	if (UWorld* World = GetWorld())
	{
		if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			Wind = Env->GetWindAtAltitude(GetActorLocation().Z) * BoostMult;
			GliderVelocity += Wind * DeltaTime;
		}
	}

	// 3. Drag
	float Speed = GliderVelocity.Size();
	FVector Drag = -GliderVelocity.GetSafeNormal() * (Speed * 0.05f);
	GliderVelocity += Drag * DeltaTime;

	// 4. Lift & Steering
	FVector GazeDirection = VRCamera->GetForwardVector();
	float Pitch = GazeDirection.Z;
	GliderVelocity += GazeDirection * (Pitch < 0 ? -Pitch * 500.f : -Pitch * 200.f) * DeltaTime;
	
	// Body Lean Steering (offset from capsule center)
	FVector CameraLocalPos = VRCamera->GetRelativeLocation();
	float LeanSteer = FMath::Clamp(CameraLocalPos.Y / 30.f, -1.f, 1.f);
	FQuat LeanRotation(FVector::UpVector, LeanSteer * 1.5f * DeltaTime);
	GliderVelocity = LeanRotation.RotateVector(GliderVelocity);

	GliderVelocity = FMath::VInterpTo(GliderVelocity, GazeDirection * Speed, DeltaTime, 2.0f);

	// Haptic Feedback for wind turbulence
	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		float Turbulence = Wind.Size() * 0.001f;
		XR->TriggerHapticVibration(true, FMath::Min(Turbulence, 0.2f), 0.1f, 30.f);
		XR->TriggerHapticVibration(false, FMath::Min(Turbulence, 0.2f), 0.1f, 30.f);
	}

	// 5. Apply movement
	FHitResult Hit;
	AddActorWorldOffset(GliderVelocity * DeltaTime, true, &Hit);

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplySmoothLocomotion(GliderVelocity * DeltaTime);
	}
}

void AMistspireVRPawn::UpdateImmersiveAudio(float DeltaTime)
{
	if (!WindAudio || !ExertionAudio) return;

	// Wind Audio based on relative velocity and environment wind
	float RelativeSpeed = GliderVelocity.Size();
	if (UWorld* World = GetWorld())
	{
		if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			RelativeSpeed += Env->GetWindAtAltitude(GetActorLocation().Z).Size();
		}
	}

	float WindVolume = FMath::Clamp(RelativeSpeed / 2000.f, 0.1f, 1.0f);
	float WindPitch = FMath::Clamp(0.8f + (RelativeSpeed / 4000.f), 0.8f, 1.5f);

	if (UMistspireAmbienceSubsystem* Amb = GetWorld()->GetSubsystem<UMistspireAmbienceSubsystem>())
	{
		WindVolume = FMath::Clamp(WindVolume + Amb->GetTensionLevel() * 0.15f, 0.f, 1.f);
		WindPitch += Amb->GetMysteryLevel() * 0.2f;
	}

	WindAudio->SetVolumeMultiplier(WindVolume);
	WindAudio->SetPitchMultiplier(WindPitch);
	if (!WindAudio->IsPlaying()) WindAudio->Play();

	// Exertion Audio when climbing or low on stamina
	float StaminaFactor = (1.0f - (CurrentStamina / MaxStamina));
	if (bIsClimbing || bIsExhausted || StaminaFactor > 0.5f)
	{
		if (!ExertionAudio->IsPlaying()) ExertionAudio->Play();
		
		float ExertionTarget = FMath::Clamp((VerticalVelocityCmPerSec / 500.f) + StaminaFactor, 0.3f, 1.2f);
		ExertionAudio->SetVolumeMultiplier(FMath::FInterpTo(ExertionAudio->VolumeMultiplier, ExertionTarget, DeltaTime, 2.f));
		ExertionAudio->SetPitchMultiplier(FMath::Lerp(0.9f, 1.3f, StaminaFactor));
	}
	else
	{
		ExertionAudio->SetVolumeMultiplier(FMath::FInterpTo(ExertionAudio->VolumeMultiplier, 0.f, DeltaTime, 1.f));
		if (ExertionAudio->VolumeMultiplier < 0.01f) ExertionAudio->Stop();
	}
}

void AMistspireVRPawn::UpdateStamina(float DeltaTime)
{
	float PressureFactor = 1.0f / CurrentAtmosphericPressure; // Harder to move in high pressure? No, harder to breathe in low pressure.
	float EffortMultiplier = FMath::Clamp(PressureFactor, 1.0f, 2.5f);

	if (UWorld* StaminaWorld = GetWorld())
	{
		if (UMistspireEnvironmentSubsystem* Env = StaminaWorld->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			if (Env->GetCurrentWeather() == EMistspireWeatherType::ElectricTurmoil)
			{
				CurrentStamina -= 3.f * DeltaTime;
			}
		}
	}

	if (bIsClimbing)
	{
		CurrentStamina -= StaminaDrainRateClimbing * EffortMultiplier * DeltaTime;
	}
	else if (bGliderActive)
	{
		float SpeedFactor = GliderVelocity.Size() / 2000.f;
		CurrentStamina -= StaminaDrainRateGliding * SpeedFactor * EffortMultiplier * DeltaTime;
	}
	else
	{
		CurrentStamina += StaminaRecoveryRate * DeltaTime;
	}

	CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);
	bIsExhausted = (CurrentStamina < 10.f);

	if (CurrentStamina <= 0.f && bIsClimbing)
	{
		StopClimb();
		VerticalVelocityCmPerSec = -300.f;
		
		if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			XR->TriggerHapticVibration(true, 1.0f, 0.2f, 200.f);
			XR->TriggerHapticVibration(false, 1.0f, 0.2f, 200.f);
		}
	}
}

void AMistspireVRPawn::UpdateOxygen(float DeltaTime)
{
	// Oxygen drains when pressure is low (< 0.5 ATM, approx 5000m)
	if (CurrentAtmosphericPressure < 0.5f)
	{
		float HypoxiaSeverity = (0.5f - CurrentAtmosphericPressure) * 2.0f; // 0 at 0.5 ATM, 1 at 0 ATM
		float Drain = OxygenDrainRateBase * HypoxiaSeverity * 5.0f;
		
		// If climbing, drain even faster
		if (bIsClimbing) Drain *= 2.0f;
		
		CurrentOxygen -= Drain * DeltaTime;
	}
	else
	{
		// Recover oxygen in thick air
		CurrentOxygen += OxygenRecoveryRate * DeltaTime;
	}

	CurrentOxygen = FMath::Clamp(CurrentOxygen, 0.f, MaxOxygen);

	// Hypoxia Effects: Screen Shake & Disorientation
	if (CurrentOxygen < 30.f)
	{
		float HypoxiaIntensity = 1.0f - (CurrentOxygen / 30.f);
		FVector Shake = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f)) * HypoxiaIntensity * 2.f;
		VRCamera->AddRelativeLocation(Shake);
		
		if (CurrentOxygen <= 0.f)
		{
			// Unconscious / Fall
			if (bIsClimbing) StopClimb();
			VerticalVelocityCmPerSec -= 500.f * DeltaTime;
		}
	}
}

void AMistspireVRPawn::UpdateAtmosphericEffects(float DeltaTime)
{
	if (UWorld* World = GetWorld())
	{
		if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			float Altitude = GetActorLocation().Z;
			CurrentAtmosphericPressure = Env->GetAtmosphericPressure(Altitude);
			
			const float Temp = Env->GetTemperatureCelsius(Altitude);
			const float Aurora = Env->GetAuroraIntensity(Altitude);
			if (ComfortVignette)
			{
				float FrostIntensity = (Temp < 0.f) ? FMath::Clamp(-Temp / 50.f, 0.f, 0.35f) : 0.f;
				float AuroraGlow = Aurora * 0.25f;
				ComfortVignette->BlendWeight = FMath::Max(ComfortVignette->BlendWeight, FrostIntensity + AuroraGlow);
			}
		}
	}
}

FVector AMistspireVRPawn::GetLeftHandWorldLocation() const
{
	return LeftHandController ? LeftHandController->GetComponentLocation() : GetActorLocation();
}

FVector AMistspireVRPawn::GetRightHandWorldLocation() const
{
	return RightHandController ? RightHandController->GetComponentLocation() : GetActorLocation();
}

float AMistspireVRPawn::GetStaminaPercent() const
{
	return MaxStamina > 0.f ? CurrentStamina / MaxStamina : 0.f;
}

float AMistspireVRPawn::GetOxygenPercent() const
{
	return MaxOxygen > 0.f ? CurrentOxygen / MaxOxygen : 0.f;
}

float AMistspireVRPawn::GetAtmosphericPressure() const
{
	return CurrentAtmosphericPressure;
}

void AMistspireVRPawn::ApplyShelterRefill(float OxygenPerSec, float StaminaPerSec, float DeltaTime)
{
	CurrentOxygen = FMath::Min(MaxOxygen, CurrentOxygen + OxygenPerSec * DeltaTime);
	CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaPerSec * DeltaTime);
	bIsExhausted = CurrentStamina < 10.f;
}

void AMistspireVRPawn::ApplyWindCrystalBoost(float DurationSeconds, float StaminaRestore)
{
	GliderBoostTimeRemaining = FMath::Max(GliderBoostTimeRemaining, DurationSeconds);
	CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRestore);
	if (!bGliderActive)
	{
		ToggleGlider(true);
	}
}

void AMistspireVRPawn::TryMantle(float DeltaTime)
{
	if (bIsClimbing || bGliderActive || !Capsule || !LeftHandController || !RightHandController)
	{
		return;
	}

	const float HandHeight = FMath::Max(
		LeftHandController->GetRelativeLocation().Z,
		RightHandController->GetRelativeLocation().Z);
	if (HandHeight < Capsule->GetScaledCapsuleHalfHeight() * 0.55f)
	{
		return;
	}

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * 90.f + FVector(0.f, 0.f, 130.f);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MistspireMantle), false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params) && Hit.bBlockingHit)
	{
		const FVector MantleDelta = FVector(0.f, 0.f, 95.f) + GetActorForwardVector() * 45.f;
		AddActorWorldOffset(MantleDelta * FMath::Min(DeltaTime * 10.f, 1.f), true);
	}
}

void AMistspireVRPawn::UpdateBeaconPulseHaptics()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UMistspireBeaconSubsystem* Beacon = World->GetSubsystem<UMistspireBeaconSubsystem>();
	UMistspireXRActionSubsystem* XR = World->GetSubsystem<UMistspireXRActionSubsystem>();
	if (!Beacon || !XR)
	{
		return;
	}

	const FMistspireBeaconTarget Target = Beacon->GetCachedBeacon();
	if (!Target.bValid || Target.DistanceCm > 150000.f)
	{
		return;
	}

	if (FMath::Sin(Beacon->GetPulsePhase()) > 0.92f)
	{
		const float Strength = FMath::Clamp(1.f - (Target.DistanceCm / 150000.f), 0.05f, 0.25f);
		XR->TriggerHapticVibration(true, Strength, 0.03f, 70.f);
	}
}

void AMistspireVRPawn::TryGrappleShot()
{
	if (bGrappleActive || !RightHandController || !VRCamera)
	{
		return;
	}

	const FVector Start = GetRightHandWorldLocation();
	const FVector End = Start + VRCamera->GetForwardVector() * GrappleTraceDistanceCm;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MistspireGrapple), false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params) && Hit.bBlockingHit)
	{
		FireGrapple(Hit.ImpactPoint);
		if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			XR->TriggerHapticVibration(false, 0.6f, 0.1f, 120.f);
		}
	}
}

void AMistspireVRPawn::UpdateWristHUD()
{
	if (StaminaWristText)
	{
		StaminaWristText->SetText(FText::Format(
			NSLOCTEXT("Mistspire", "StaminaFmt", "STA {0}%"),
			FText::AsNumber(FMath::RoundToInt(100.f * CurrentStamina / MaxStamina))));
	}
	if (OxygenWristText)
	{
		OxygenWristText->SetText(FText::Format(
			NSLOCTEXT("Mistspire", "OxygenFmt", "O2 {0}%"),
			FText::AsNumber(FMath::RoundToInt(100.f * CurrentOxygen / MaxOxygen))));
	}
	if (BeaconWristText && GetWorld())
	{
		if (UMistspireBeaconSubsystem* Beacon = GetWorld()->GetSubsystem<UMistspireBeaconSubsystem>())
		{
			const FMistspireBeaconTarget Target = Beacon->GetCachedBeacon();
			if (Target.bValid)
			{
				const float Km = Target.DistanceCm / 100000.f;
				BeaconWristText->SetText(FText::Format(
					NSLOCTEXT("Mistspire", "BeaconFmt", "▲ {0}km {1}°"),
					FText::AsNumber(FMath::RoundToInt(Km * 10.f) / 10.f),
					FText::AsNumber(FMath::RoundToInt(Target.BearingDegrees))));
			}
			else
			{
				BeaconWristText->SetText(NSLOCTEXT("Mistspire", "BeaconDone", "▲ ALL SUMMITS"));
			}
		}
	}

	if (GliderBoostTimeRemaining > 0.f && StaminaWristText)
	{
		StaminaWristText->SetText(FText::Format(
			NSLOCTEXT("Mistspire", "BoostFmt", "BOOST {0}s"),
			FText::AsNumber(FMath::CeilToInt(GliderBoostTimeRemaining))));
	}
}

void AMistspireVRPawn::HandleSummitReached(FName SummitId)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
		{
			Progress->CaptureProgressFromWorld(GetWorld());
		}
	}

	if (SummitChimeAudio)
	{
		SummitChimeAudio->Play();
	}

	if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
	{
		for (int32 i = 0; i < 3; ++i)
		{
			XR->TriggerHapticVibration(true, 0.5f, 0.15f, 200.f - i * 40.f);
			XR->TriggerHapticVibration(false, 0.5f, 0.15f, 200.f - i * 40.f);
		}
	}

	CurrentStamina = MaxStamina;
	CurrentOxygen = FMath::Min(MaxOxygen, CurrentOxygen + 25.f);
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
	bGrappleActive = true;
	GrappleAnchorPoint = WorldTarget;
	
	if (GrappleCable)
	{
		GrappleCable->SetHiddenInGame(false);
		GrappleCable->SetWorldLocation(WorldTarget);
	}

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
	
	if (GliderMesh)
	{
		GliderMesh->SetHiddenInGame(!bEnable);
	}

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

		// Social Altitude Sync (Multiplayer Leaderboard)
		if (AMistspirePlayerState* PS = GetPlayerState<AMistspirePlayerState>())
		{
			PS->UpdateAltitude(Alt->GetCurrentAltitudeCm());
		}
	}

	if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
	{
		static const FName SummitIds[] = {
			TEXT("summit_valley_gate"),
			TEXT("summit_mesa_crown"),
			TEXT("summit_cloud_garden"),
			TEXT("summit_obelisk_prime"),
			TEXT("summit_orbital_needle"),
			TEXT("summit_spire_cathedral"),
			TEXT("summit_rift_observatory"),
			TEXT("summit_ember_crown"),
		};
		for (FName Id : SummitIds)
		{
			if (Registry->TryReachSummit(Id, GetActorLocation()))
			{
				if (AMistspirePlayerState* PS = GetPlayerState<AMistspirePlayerState>())
				{
					PS->AddSummit(Id);
				}
			}
		}
	}
}
