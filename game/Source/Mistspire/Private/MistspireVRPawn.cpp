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
#include "MistspireAudioSubsystem.h"
#include "MistspireInputMode.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireGameUserSettings.h"
#include "MistspireSettingsPanel.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireLoreShard.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PostProcessComponent.h"
#include "CableComponent.h"
#include "MotionControllerComponent.h"
#include "IMotionController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "CollisionShape.h"
#include "CollisionQueryParams.h"
#include "Widgets/SOverlay.h"

namespace
{
	constexpr float MaxLocomotionRpcDeltaCm = 2500.f;
	constexpr float MaxTeleportRpcDistanceCm = 2500.f;
	constexpr float NonVRGroundSkinCm = 0.5f;
	constexpr float NonVRGroundContactGapCm = 2.f;
	constexpr float NonVRGroundProbeRadiusCm = 4.f;
	/** Ignore tiny Z corrections (stops jitter between rock contact points). */
	constexpr float NonVRGroundSnapDeadzoneCm = 1.75f;
	/** Stationary: ignore support Z changes smaller than this (pebbles/debris). */
	constexpr float NonVRStationarySupportStickCm = 8.f;
	/** Treat as standing still for support sticking (cm/s). */
	constexpr float NonVRStationarySpeedCmPerSec = 35.f;
	/** Falling faster than this skips walk-follow snaps so jumps can finish. */
	constexpr float NonVRFallFollowCancelCmPerSec = -120.f;
}

AMistspireVRPawn::AMistspireVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(34.f, 88.f);
	Capsule->SetCollisionProfileName(TEXT("Pawn"));
	Capsule->SetIsReplicated(true);
	SetRootComponent(Capsule);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(Capsule);
	VRCamera->bUsePawnControlRotation = false;

	ComfortVignette = CreateDefaultSubobject<UPostProcessComponent>(TEXT("ComfortVignette"));
	ComfortVignette->SetupAttachment(VRCamera);

	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(Capsule);
	LeftHandController->MotionSource = IMotionController::LeftHandSourceId;

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

	BiomeWristText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("BiomeWristText"));
	BiomeWristText->SetupAttachment(VisualLeftHand);
	BiomeWristText->SetRelativeLocation(FVector(0.f, 8.f, -6.f));
	BiomeWristText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	BiomeWristText->SetHorizontalAlignment(EHTA_Center);
	BiomeWristText->SetWorldSize(2.5f);

	NotificationText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NotificationText"));
	NotificationText->SetupAttachment(VRCamera);
	NotificationText->SetRelativeLocation(FVector(60.f, 0.f, -30.f));
	NotificationText->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	NotificationText->SetHorizontalAlignment(EHTA_Center);
	NotificationText->SetWorldSize(3.f);
	NotificationText->SetText(FText::GetEmpty());
	NotificationText->SetHiddenInGame(true);

	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(Capsule);
	RightHandController->MotionSource = IMotionController::RightHandSourceId;

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
	GrappleCable->bAttachStart = true;
	GrappleCable->bAttachEnd = true;
	GrappleCable->EndLocation = FVector::ZeroVector;
	GrappleCable->CableLength = 100.f;
	GrappleCable->NumSegments = 1;
	GrappleCable->SolverIterations = 16;
	GrappleCable->bEnableStiffness = true;
	GrappleCable->CableGravityScale = 0.f;
	GrappleCable->CableWidth = 4.f;
	GrappleCable->bUseSubstepping = true;
	GrappleCable->SubstepTime = 0.005f;

	GrappleAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("GrappleAnchor"));
	GrappleAnchor->SetupAttachment(Capsule);
	GrappleCable->SetAttachEndToComponent(GrappleAnchor);

	GliderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GliderMesh"));
	GliderMesh->SetupAttachment(VRCamera);
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
	bNonVRMode = FMistspireInputMode::IsNonVRMode(GetWorld());
	FMistspireInputMode::ApplyRendererOverrides(bNonVRMode);

	if (Capsule)
	{
		Capsule->SetGenerateOverlapEvents(true);
	}

	if (bNonVRMode)
	{
		bGameplayStarted = false;
		ConfigureNonVRMode();
	}
	else
	{
		bGameplayStarted = true;
	}

	if (UWorld* World = GetWorld())
	{
		if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
		{
			Registry->OnSummitReached.AddDynamic(this, &AMistspireVRPawn::HandleSummitReached);
		}
	}
}

void AMistspireVRPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ApplyNonVRPlayerControllerSettings();
}

void AMistspireVRPawn::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	ApplyNonVRPlayerControllerSettings();
}

void AMistspireVRPawn::StartGameplay()
{
	if (bGameplayStarted)
	{
		return;
	}

	bGameplayStarted = true;
	ApplyNonVRPlayerControllerSettings();
	ApplyUserSettingsToGameplay();
}

void AMistspireVRPawn::ApplyUserSettingsToGameplay()
{
	if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Settings->ApplyGameplaySettings(this);
	}
}

void AMistspireVRPawn::ToggleSettingsMenu()
{
	if (!bNonVRMode || !bGameplayStarted)
	{
		return;
	}

	if (bSettingsMenuOpen)
	{
		CloseSettingsMenu(true);
	}
	else
	{
		OpenSettingsMenu();
	}
}

void AMistspireVRPawn::OpenSettingsMenu()
{
	if (bSettingsMenuOpen || !GEngine || !GEngine->GameViewport)
	{
		return;
	}

	bSettingsMenuOpen = true;
	NonVRMoveForward = 0.f;
	NonVRMoveRight = 0.f;
	CachedMoveInput = FVector2D::ZeroVector;
	HorizontalVelocity = FVector::ZeroVector;
	bNonVRClimbHeld = false;
	bNonVRSprintHeld = false;

	TSharedRef<SMistspireSettingsPanel> Panel = SNew(SMistspireSettingsPanel)
		.OwnerPawn(this)
		.OnClosed(FOnMistspireSettingsClosed::CreateLambda([this]()
		{
			CloseSettingsMenu(true);
		}));

	TSharedRef<SWidget> Content =
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			Panel
		];

	SettingsMenuWidget = Content;
	GEngine->GameViewport->AddViewportWidgetContent(Content, 100);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = true;
		// GameAndUI keeps Escape reachable; UIOnly sets viewport IgnoreInput and blocks Esc close.
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(Panel);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		FSlateApplication::Get().SetKeyboardFocus(Panel, EFocusCause::SetDirectly);
	}
}

void AMistspireVRPawn::CloseSettingsMenu(bool bSaveSettings)
{
	if (!bSettingsMenuOpen)
	{
		return;
	}

	bSettingsMenuOpen = false;

	if (GEngine && GEngine->GameViewport && SettingsMenuWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(SettingsMenuWidget.ToSharedRef());
	}
	SettingsMenuWidget.Reset();

	if (bSaveSettings)
	{
		if (UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
		{
			Settings->ApplyGameplaySettings(this);
			Settings->SaveSettings();
		}
	}

	ApplyNonVRPlayerControllerSettings();
}

void AMistspireVRPawn::PollSettingsMenuToggle()
{
	if (!bNonVRMode || !bGameplayStarted || bSettingsMenuOpen)
	{
		// Esc while open is handled by SMistspireSettingsPanel::OnKeyDown.
		return;
	}

	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->WasInputKeyJustPressed(EKeys::Escape))
	{
		return;
	}

	OpenSettingsMenu();
}

bool AMistspireVRPawn::TryConsumeStartScreenInput() const
{
	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return false;
	}

	return PC->WasInputKeyJustPressed(EKeys::AnyKey)
		|| PC->WasInputKeyJustPressed(EKeys::LeftMouseButton)
		|| PC->WasInputKeyJustPressed(EKeys::RightMouseButton)
		|| PC->WasInputKeyJustPressed(EKeys::MiddleMouseButton);
}

void AMistspireVRPawn::ApplyNonVRPlayerControllerSettings()
{
	if (!bNonVRMode || !bGameplayStarted || bSettingsMenuOpen)
	{
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		FMistspireInputMode::AddNonVRMappingContext(PC, NonVRMappingContext);
	}
}

void AMistspireVRPawn::ConfigureNonVRMode()
{
	bUseControllerRotationYaw = true;

	if (VRCamera)
	{
		VRCamera->bUsePawnControlRotation = true;
		VRCamera->SetRelativeLocation(FVector(0.f, 0.f, NonVREyeHeightCm));
	}

	if (VisualLeftHand) VisualLeftHand->SetHiddenInGame(true, true);
	if (VisualRightHand) VisualRightHand->SetHiddenInGame(true, true);
	if (FullBodyMesh) FullBodyMesh->SetHiddenInGame(true, true);
	if (LeftHandController) LeftHandController->SetHiddenInGame(true, true);
	if (RightHandController) RightHandController->SetHiddenInGame(true, true);

	if (ComfortVignette)
	{
		ComfortVignette->bEnabled = false;
		ComfortVignette->BlendWeight = 0.f;
	}

	if (NotificationText)
	{
		NotificationText->SetHiddenInGame(true);
	}

	if (GrappleCable && VRCamera)
	{
		GrappleCable->AttachToComponent(VRCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		// Fallback; while grappling PlaceNonVRGrappleCableStart pushes the root past the bottom of the view.
		GrappleCable->SetRelativeLocation(FVector(40.f, 45.f, -160.f));
	}

	ApplyNonVRPlayerControllerSettings();
	ApplyUserSettingsToGameplay();
}

FVector AMistspireVRPawn::GetInteractionTraceStart() const
{
	if (VRCamera)
	{
		return VRCamera->GetComponentLocation();
	}
	return GetActorLocation();
}

FVector AMistspireVRPawn::GetInteractionTraceEnd(float MaxDistanceCm) const
{
	const FVector Forward = VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector();
	return GetInteractionTraceStart() + Forward * MaxDistanceCm;
}

void AMistspireVRPawn::PollNonVRInput()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	CachedMoveInput = FVector2D(NonVRMoveRight, NonVRMoveForward);
	CachedTurnInput = 0.f;

	float Speed = DefaultLocomotionSpeedCmPerSec;
	if (bIsClimbing || bNonVRClimbHeld)
	{
		Speed = DefaultLocomotionSpeedCmPerSec * 0.55f;
	}
	else if (bGliderActive)
	{
		Speed = DefaultLocomotionSpeedCmPerSec * 1.5f;
	}
	else if (bNonVRSprintHeld)
	{
		Speed = SprintSpeedCmPerSec;
	}
	LocomotionSpeedCmPerSec = Speed;

	if (bNonVRClimbHeld)
	{
		const FVector TraceStart = GetInteractionTraceStart();
		const FVector TraceEnd = TraceStart + (VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector()) * 120.f;
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(NonVRClimbTrace), false, this);
		const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params)
			&& Hit.bBlockingHit;
		// Only vertical-ish surfaces count as climbable (not the boulder top / ground).
		const bool bNearClimbable = bHit && Hit.ImpactNormal.Z < 0.55f;

		if (bNearClimbable)
		{
			NonVRClimbMissFrames = 0;
			if (!bIsClimbing)
			{
				StartClimb();
			}
		}
		else if (bIsClimbing)
		{
			++NonVRClimbMissFrames;
			// Crest / look-away flicker: require several misses before dropping off the wall.
			if (NonVRClimbMissFrames >= 8)
			{
				StopClimb();
			}
		}
	}
	else if (bIsClimbing)
	{
		StopClimb();
	}
}

void AMistspireVRPawn::OnClimbPressed()
{
	if (!bNonVRMode || !bGameplayStarted || bSettingsMenuOpen) return;
	bNonVRClimbHeld = true;
}
void AMistspireVRPawn::OnClimbReleased()
{
	if (!bNonVRMode) return;
	bNonVRClimbHeld = false;
	if (bIsClimbing) StopClimb();
}
void AMistspireVRPawn::OnSprintPressed()
{
	if (!bNonVRMode || !bGameplayStarted || bSettingsMenuOpen) return;
	bNonVRSprintHeld = true;
}
void AMistspireVRPawn::OnSprintReleased()
{
	if (!bNonVRMode) return;
	bNonVRSprintHeld = false;
}
void AMistspireVRPawn::OnGrapplePressed()
{
	if (bNonVRMode && bGameplayStarted && !bSettingsMenuOpen)
	{
		TryGrappleShot();
	}
}
void AMistspireVRPawn::OnGliderPressed() { if (bNonVRMode && bGameplayStarted && !bSettingsMenuOpen) ToggleGlider(!bGliderActive); }
void AMistspireVRPawn::OnTeleportPressed() { if (bNonVRMode && bGameplayStarted && !bSettingsMenuOpen) TeleportForward(TeleportForwardCm); }
void AMistspireVRPawn::OnInteractPressed()
{
	if (!bNonVRMode || !bGameplayStarted || bSettingsMenuOpen)
	{
		return;
	}
	if (UMistspireInteractionSubsystem* Interact = GetWorld()->GetSubsystem<UMistspireInteractionSubsystem>())
	{
		Interact->TryInteractFromPawn(this);
	}
}

void AMistspireVRPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseSettingsMenu(false);
	if (UWorld* World = GetWorld())
	{
		if (UMistspireSummitRegistry* Registry = World->GetSubsystem<UMistspireSummitRegistry>())
		{
			Registry->OnSummitReached.RemoveDynamic(this, &AMistspireVRPawn::HandleSummitReached);
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
		if (bNonVRMode && !bGameplayStarted)
		{
			if (TryConsumeStartScreenInput())
			{
				StartGameplay();
			}
			else
			{
				CachedMoveInput = FVector2D::ZeroVector;
				NonVRMoveForward = 0.f;
				NonVRMoveRight = 0.f;
				HorizontalVelocity = FVector::ZeroVector;
				UpdateNonVRGravity(DeltaTime);
				UpdateAltitudeTracking();
				return;
			}
		}

		PollSettingsMenuToggle();

		if (bSettingsMenuOpen)
		{
			CachedMoveInput = FVector2D::ZeroVector;
			NonVRMoveForward = 0.f;
			NonVRMoveRight = 0.f;
			HorizontalVelocity = FVector::ZeroVector;
			UpdateAltitudeTracking();
			return;
		}

		if (bNonVRMode)
		{
			PollNonVRInput();
		}
		else
		{
			PollXRInput();
		}
		
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
			if (bNonVRMode)
			{
				// Grapple owns vertical motion — gravity/ground snap would pin you to rocks.
				if (!bGrappleActive && !bGrappleExtending)
				{
					UpdateNonVRGravity(DeltaTime);
				}
			}
			else
			{
				ApplyVerticalVelocity(VerticalVelocityCmPerSec * DeltaTime);
				VerticalVelocityCmPerSec = FMath::FInterpTo(VerticalVelocityCmPerSec, 0.f, DeltaTime, 4.f);
			}
		}

		if (bNonVRMode)
		{
			UpdateNonVRCameraBob(DeltaTime);
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

		if (NotificationTimer > 0.f)
		{
			NotificationTimer -= DeltaTime;
			if (NotificationText)
			{
				const float Alpha = FMath::Min(NotificationTimer, 1.f);
				NotificationText->SetTextRenderColor(FLinearColor(1.f, 1.f, 1.f, Alpha).ToFColor(false));
			}
			if (NotificationTimer <= 0.f && NotificationText)
			{
				NotificationText->SetText(FText::GetEmpty());
				NotificationText->SetHiddenInGame(true);
			}
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

		// Comfort Vignette based on rotation, speed, exhaustion, and hypoxia (VR only)
		if (ComfortVignette && !bNonVRMode)
		{
			float TurnFactor = FMath::Abs(CachedTurnInput);
			float SpeedFactor = GliderVelocity.Size() / 4000.f;
			float ExhaustionFactor = bIsExhausted ? 0.4f : (1.0f - (CurrentStamina / MaxStamina)) * 0.3f;
			float HypoxiaFactor = (1.0f - (CurrentOxygen / MaxOxygen)) * 0.8f;
			float Intensity = FMath::Max(FMath::Max(FMath::Max(TurnFactor, SpeedFactor * 0.5f), ExhaustionFactor), HypoxiaFactor);
			ComfortVignette->BlendWeight = FMath::FInterpTo(ComfortVignette->BlendWeight, Intensity, DeltaTime, 5.f);
		}

		// Grapple: extend tip to world anchor, then pull (hold trigger to reel faster)
		if (bGrappleActive)
		{
			UpdateGrapple(DeltaTime);
		}

		// Physical Hand Collisions (VR only)
		if (!bNonVRMode)
		{
		auto UpdateHandPhysics = [&](USkeletalMeshComponent* VisualHand, UMotionControllerComponent* HandController)
		{
			if (!VisualHand || !HandController) return;
			
			FVector TargetLoc = HandController->GetComponentLocation();
			FRotator TargetRot = HandController->GetComponentRotation();
			
			FHitResult Hit;
			VisualHand->SetWorldLocationAndRotation(TargetLoc, TargetRot, true, &Hit);
			
		// If we hit something, triggered haptics and surface contact audio to feel the "bump"
		if (Hit.bBlockingHit)
		{
			if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
			{
				bool bIsLeft = (VisualHand == VisualLeftHand);
				XR->TriggerHapticVibration(bIsLeft, 0.3f, 0.05f, 100.f);
			}
			if (UMistspireAudioSubsystem* Audio = GetWorld()->GetSubsystem<UMistspireAudioSubsystem>())
			{
				Audio->PlaySurfaceContactSound(500.f, Hit.PhysMaterial.IsValid());
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

	bNonVRMode = FMistspireInputMode::IsNonVRMode(GetWorld());
	if (!bNonVRMode)
	{
		return;
	}

	BindNonVREnhancedInput(Cast<UEnhancedInputComponent>(PlayerInputComponent));
	ApplyNonVRPlayerControllerSettings();
}

void AMistspireVRPawn::BindNonVREnhancedInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Mistspire non-VR requires EnhancedInputComponent (DefaultInput.ini DefaultInputComponentClass)."));
		return;
	}

	if (!NonVRMappingContext || !NonVRMoveAction)
	{
		FMistspireNonVREnhancedInput Built;
		FMistspireInputMode::CreateNonVREnhancedInput(this, Built);
		NonVRMappingContext = Built.MappingContext;
		NonVRMoveAction = Built.Move;
		NonVRLookAction = Built.Look;
		NonVRJumpAction = Built.Jump;
		NonVRClimbAction = Built.Climb;
		NonVRSprintAction = Built.Sprint;
		NonVRGrappleAction = Built.Grapple;
		NonVRGliderAction = Built.Glider;
		NonVRTeleportAction = Built.Teleport;
		NonVRInteractAction = Built.Interact;
	}

	if (!NonVRMappingContext || !NonVRMoveAction || !NonVRLookAction)
	{
		return;
	}

	EnhancedInputComponent->BindAction(NonVRMoveAction, ETriggerEvent::Triggered, this, &AMistspireVRPawn::OnNonVRMove);
	EnhancedInputComponent->BindAction(NonVRMoveAction, ETriggerEvent::Completed, this, &AMistspireVRPawn::OnNonVRMove);
	EnhancedInputComponent->BindAction(NonVRLookAction, ETriggerEvent::Triggered, this, &AMistspireVRPawn::OnNonVRLook);
	EnhancedInputComponent->BindAction(NonVRJumpAction, ETriggerEvent::Started, this, &AMistspireVRPawn::TryJump);
	EnhancedInputComponent->BindAction(NonVRClimbAction, ETriggerEvent::Started, this, &AMistspireVRPawn::OnClimbPressed);
	EnhancedInputComponent->BindAction(NonVRClimbAction, ETriggerEvent::Completed, this, &AMistspireVRPawn::OnClimbReleased);
	EnhancedInputComponent->BindAction(NonVRSprintAction, ETriggerEvent::Started, this, &AMistspireVRPawn::OnSprintPressed);
	EnhancedInputComponent->BindAction(NonVRSprintAction, ETriggerEvent::Completed, this, &AMistspireVRPawn::OnSprintReleased);
	EnhancedInputComponent->BindAction(NonVRGrappleAction, ETriggerEvent::Started, this, &AMistspireVRPawn::OnGrapplePressed);
	EnhancedInputComponent->BindAction(NonVRGliderAction, ETriggerEvent::Started, this, &AMistspireVRPawn::OnGliderPressed);
	EnhancedInputComponent->BindAction(NonVRTeleportAction, ETriggerEvent::Started, this, &AMistspireVRPawn::OnTeleportPressed);
	EnhancedInputComponent->BindAction(NonVRInteractAction, ETriggerEvent::Started, this, &AMistspireVRPawn::OnInteractPressed);
}

void AMistspireVRPawn::OnNonVRMove(const FInputActionValue& Value)
{
	if (!bGameplayStarted || bSettingsMenuOpen)
	{
		NonVRMoveRight = 0.f;
		NonVRMoveForward = 0.f;
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	NonVRMoveRight = Axis.X;
	NonVRMoveForward = Axis.Y;
}

void AMistspireVRPawn::OnNonVRLook(const FInputActionValue& Value)
{
	if (!bNonVRMode || !bGameplayStarted || bSettingsMenuOpen)
	{
		return;
	}

	float Sensitivity = 1.f;
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		Sensitivity = Settings->GetMouseSensitivity();
	}

	const FVector2D Axis = Value.Get<FVector2D>() * Sensitivity;
	if (!FMath::IsNearlyZero(Axis.X))
	{
		AddControllerYawInput(Axis.X);
	}
	if (!FMath::IsNearlyZero(Axis.Y))
	{
		AddControllerPitchInput(Axis.Y);
	}
}

void AMistspireVRPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMistspireVRPawn, bIsClimbing);
	DOREPLIFETIME(AMistspireVRPawn, bGliderActive);
	DOREPLIFETIME(AMistspireVRPawn, bGrappleActive);
	DOREPLIFETIME(AMistspireVRPawn, GrappleAnchorPoint);
	DOREPLIFETIME(AMistspireVRPawn, CurrentStamina);
	DOREPLIFETIME(AMistspireVRPawn, CurrentOxygen);
}

void AMistspireVRPawn::ShowNotification(const FString& Message, float Duration)
{
	if (bNonVRMode)
	{
		if (UWorld* World = GetWorld())
		{
			if (UMistspireNarrativeSubsystem* Narr = World->GetSubsystem<UMistspireNarrativeSubsystem>())
			{
				Narr->PushLine(FText::FromString(Message), Duration);
			}
		}
		return;
	}

	if (!NotificationText)
	{
		return;
	}

	NotificationText->SetHiddenInGame(false);
	NotificationText->SetText(FText::FromString(Message));
	NotificationTimer = Duration;
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

	if (State.bMenuPressed)
	{
		if (!bMenuPressedLast)
		{
			bMenuHeld = true;
			MenuHoldTimer = 0.f;
			if (UMistspireAudioSubsystem* Audio = World->GetSubsystem<UMistspireAudioSubsystem>())
			{
				Audio->PlayUISound(TEXT("ui_menu_press"), 0.5f);
			}
		}
		MenuHoldTimer += GetWorld()->GetDeltaSeconds();
		if (MenuHoldTimer >= 1.0f && bMenuHeld)
		{
			bMenuHeld = false;
			ToggleGlider(!bGliderActive);
			if (UMistspireAudioSubsystem* Audio = World->GetSubsystem<UMistspireAudioSubsystem>())
			{
				Audio->PlayUISound(bGliderActive ? TEXT("ui_glider_deploy") : TEXT("ui_glider_stow"), 0.6f);
			}
		}
	}
	else
	{
		if (bMenuPressedLast && bMenuHeld && MenuHoldTimer < 1.0f)
		{
			TeleportForward(TeleportForwardCm);
			if (UMistspireAudioSubsystem* Audio = World->GetSubsystem<UMistspireAudioSubsystem>())
			{
				Audio->PlayUISound(TEXT("ui_teleport"), 0.5f);
			}
		}
		bMenuHeld = false;
		MenuHoldTimer = 0.f;
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
	const bool bHasMoveInput = !MoveInput.IsNearlyZero(0.01f);

	const float YawDeg = bNonVRMode
		? GetControlRotation().Yaw
		: (GetActorRotation().Yaw + CachedTurnInput * TurnRateDegPerSec * DeltaTime);
	const FRotator YawRot(0.f, YawDeg, 0.f);

	FVector DesiredVelocity = FVector::ZeroVector;
	if (bHasMoveInput)
	{
		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		FVector Wish = Forward * MoveInput.Y + Right * MoveInput.X;
		Wish.Z = 0.f;
		Wish = Wish.GetClampedToMaxSize(1.f);
		DesiredVelocity = Wish * LocomotionSpeedCmPerSec;
	}

	const float InterpSpeed = bHasMoveInput ? LocomotionAccelInterp : LocomotionBrakeInterp;
	HorizontalVelocity = FMath::VInterpTo(HorizontalVelocity, DesiredVelocity, DeltaTime, InterpSpeed);
	HorizontalVelocity.Z = 0.f;

	if (HorizontalVelocity.SizeSquared() < 1.f)
	{
		HorizontalVelocity = FVector::ZeroVector;
	}

	if (bNonVRMode)
	{
		FRotator BodyRot = GetActorRotation();
		BodyRot.Yaw = YawDeg;
		SetActorRotation(BodyRot);
	}

	if (HorizontalVelocity.IsNearlyZero())
	{
		return;
	}

	const FVector Delta = HorizontalVelocity * DeltaTime;
	if (bNonVRMode)
	{
		ApplyNonVRWalkDelta(Delta);
	}
	else
	{
		FHitResult Hit;
		AddActorWorldOffset(Delta, true, &Hit);
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplySmoothLocomotion(Delta);
	}
}

bool AMistspireVRPawn::Server_ApplySmoothLocomotion_Validate(FVector Delta)
{
	return Delta.SizeSquared() <= FMath::Square(MaxLocomotionRpcDeltaCm);
}
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
	if (bNonVRMode)
	{
		ClearNonVRGroundCache();
		VerticalVelocityCmPerSec = 0.f;

		const float ClimbSpeed = LocomotionSpeedCmPerSec;
		FVector ClimbDelta(0.f, 0.f, ClimbSpeed * DeltaTime);

		const FVector Forward = VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector();
		const FVector FlatFwd = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();

		// Always ease slightly into the wall so we stay attached; W adds more forward cresting.
		const float IntoWall = FMath::IsNearlyZero(NonVRMoveForward, 0.01f) ? 0.25f : NonVRMoveForward;
		if (!FlatFwd.IsNearlyZero() && IntoWall > 0.f)
		{
			ClimbDelta += FlatFwd * IntoWall * ClimbSpeed * 0.55f * DeltaTime;
		}

		FHitResult Hit;
		AddActorWorldOffset(ClimbDelta, true, &Hit);

		if (Hit.bBlockingHit)
		{
			// Hit a walkable top while rising — mantle onto it instead of vibrating on the lip.
			if (IsFloorHit(Hit))
			{
				SnapFeetToGround(Hit);
				if (!FlatFwd.IsNearlyZero())
				{
					AddActorWorldOffset(FlatFwd * 35.f, true);
				}
				StopClimb();
				return;
			}

			const FVector Remaining = ClimbDelta * (1.f - Hit.Time);
			const FVector Slide = FVector::VectorPlaneProject(Remaining, Hit.ImpactNormal);
			if (!Slide.IsNearlyZero(0.5f))
			{
				AddActorWorldOffset(Slide, true);
			}
		}

		if (TryNonVRClimbMantle())
		{
			StopClimb();
			return;
		}

		if (GetLocalRole() < ROLE_Authority)
		{
			Server_ApplySmoothLocomotion(ClimbDelta);
		}
		return;
	}

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

	// Spatial glider wind audio
	if (UMistspireAudioSubsystem* Audio = GetWorld()->GetSubsystem<UMistspireAudioSubsystem>())
	{
		float AirSpeed = GliderVelocity.Size() / 100.f;
		Audio->PlaySpatialSoundAtLocation(TEXT("glider_wind"), GetActorLocation(), FMath::Clamp(AirSpeed / 20.f, 0.f, 1.f));
	}

	// 5. Apply movement
	FHitResult Hit;
	AddActorWorldOffset(GliderVelocity * DeltaTime, true, &Hit);

	if (bNonVRMode)
	{
		if (IsFloorHit(Hit))
		{
			SnapFeetToGround(Hit);
			GliderVelocity.Z = FMath::Max(0.f, GliderVelocity.Z);
			ToggleGlider(false);
		}
		else
		{
			EnforceNonVRGroundConstraint(false);
			if (IsGrounded())
			{
				GliderVelocity.Z = FMath::Max(0.f, GliderVelocity.Z);
				ToggleGlider(false);
			}
		}
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplySmoothLocomotion(GliderVelocity * DeltaTime);
	}
}

void AMistspireVRPawn::UpdateImmersiveAudio(float DeltaTime)
{
	if (!WindAudio || !ExertionAudio) return;

	UWorld* World = GetWorld();
	if (!World) return;

	UMistspireAudioSubsystem* AudioSys = World->GetSubsystem<UMistspireAudioSubsystem>();
	UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>();
	UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>();

	const float AltitudeCm = Alt ? Alt->GetCurrentAltitudeCm() : GetActorLocation().Z;

	// Altitude EQ via audio subsystem
	if (AudioSys)
	{
		AudioSys->SetAltitudeEQ(AltitudeCm / 100000.f);
	}

	// Wind Audio based on relative velocity and environment wind
	float RelativeSpeed = GliderVelocity.Size();
	if (Env)
	{
		RelativeSpeed += Env->GetWindAtAltitude(AltitudeCm).Size();
	}

	float WindVolume = FMath::Clamp(RelativeSpeed / 2000.f, 0.1f, 1.0f);
	float WindPitch = FMath::Clamp(0.8f + (RelativeSpeed / 4000.f), 0.8f, 1.5f);

	if (UMistspireAmbienceSubsystem* Amb = World->GetSubsystem<UMistspireAmbienceSubsystem>())
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

	// Biome ambience and reverb through audio subsystem
	if (AudioSys && Env)
	{
		const EMistspireBiomeType Biome = Env->GetCurrentBiome();
		if (Biome != EMistspireBiomeType::None)
		{
			FName BiomeName;
			FName ReverbName;
			switch (Biome)
			{
				case EMistspireBiomeType::Mist:
					BiomeName = TEXT("ambient_mist");
					ReverbName = TEXT("reverb_mist");
					break;
				case EMistspireBiomeType::Arid:
					BiomeName = TEXT("ambient_arid");
					ReverbName = TEXT("reverb_arid");
					break;
				case EMistspireBiomeType::Forest:
					BiomeName = TEXT("ambient_forest");
					ReverbName = TEXT("reverb_forest");
					break;
				case EMistspireBiomeType::Ember:
					BiomeName = TEXT("ambient_ember");
					ReverbName = TEXT("reverb_ember");
					break;
				case EMistspireBiomeType::Crystal:
					BiomeName = TEXT("ambient_crystal");
					ReverbName = TEXT("reverb_crystal");
					break;
			case EMistspireBiomeType::Void:
				BiomeName = TEXT("ambient_void");
				ReverbName = TEXT("reverb_void");
				break;
			case EMistspireBiomeType::Tundra:
				BiomeName = TEXT("ambient_tundra");
				ReverbName = TEXT("reverb_tundra");
				break;
			case EMistspireBiomeType::Aether:
				BiomeName = TEXT("ambient_aether");
				ReverbName = TEXT("reverb_aether");
				break;
			case EMistspireBiomeType::Sanctum:
				BiomeName = TEXT("ambient_sanctum");
				ReverbName = TEXT("reverb_sanctum");
				break;
			case EMistspireBiomeType::Pinnacle:
				BiomeName = TEXT("ambient_pinnacle");
				ReverbName = TEXT("reverb_pinnacle");
				break;
			default: break;
			}
			if (!BiomeName.IsNone())
			{
				AudioSys->PlayBiomeAmbience(BiomeName, 2.f);
			}
			if (!ReverbName.IsNone())
			{
				AudioSys->SetReverbPreset(ReverbName, 2.f);
			}
		}
	}

	// Tension level from survival factors
	if (AudioSys)
	{
		float SpeedFactor = GliderVelocity.Size() / 3000.f;
		float AltitudeFactor = AltitudeCm / 500000.f;
		float ExhaustFactor = 1.f - (CurrentStamina / MaxStamina);
		float HypoxiaFactor = 1.f - (CurrentOxygen / MaxOxygen);
		float Tension = FMath::Clamp(SpeedFactor + AltitudeFactor + ExhaustFactor + HypoxiaFactor, 0.f, 1.f);
		AudioSys->SetTensionLevel(Tension);
	}

	// Physiology sounds (timered to avoid spam)
	PhysTimer += DeltaTime;
	if (AudioSys && PhysTimer >= 2.f)
	{
		PhysTimer = 0.f;
		const float StamPct = CurrentStamina / MaxStamina;
		const float OxyPct = CurrentOxygen / MaxOxygen;

		if (OxyPct < 0.15f)
		{
			AudioSys->PlayPhysiologySound(EPhysiologySoundType::HypoxiaGasp, 1.f);
		}
		else if (OxyPct < 0.3f)
		{
			AudioSys->PlayPhysiologySound(EPhysiologySoundType::BreathingShallow, 1.f - OxyPct);
		}

		if (StamPct < 0.2f)
		{
			AudioSys->PlayPhysiologySound(EPhysiologySoundType::Exhaustion, 1.f - StamPct);
		}
		else if (StamPct < 0.4f)
		{
			AudioSys->PlayPhysiologySound(EPhysiologySoundType::BreathingHeavy, 1.f - StamPct);
		}

		const float Tension = FMath::Clamp((1.f - StamPct) * 0.5f + (1.f - OxyPct) * 0.5f, 0.f, 1.f);
		if (Tension > 0.6f)
		{
			AudioSys->PlayPhysiologySound(EPhysiologySoundType::HeartbeatRacing, Tension);
		}
	}

	// Weather audio
	if (AudioSys && Env)
	{
		AudioSys->PlayWeatherSound(Env->GetCurrentWeather(), 1.f);
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
			if (ComfortVignette && !bNonVRMode)
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
	if (bNonVRMode)
	{
		const FVector Forward = VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector();
		const FVector Origin = GetInteractionTraceStart();
		return Origin + Forward * 50.f - FVector(15.f, 0.f, 0.f);
	}
	return LeftHandController ? LeftHandController->GetComponentLocation() : GetActorLocation();
}

FVector AMistspireVRPawn::GetRightHandWorldLocation() const
{
	if (bNonVRMode)
	{
		const FVector Forward = VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector();
		const FVector Origin = GetInteractionTraceStart();
		return Origin + Forward * 50.f + FVector(15.f, 0.f, 0.f);
	}
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
	if (bNonVRMode || bIsClimbing || bGliderActive || !Capsule)
	{
		return;
	}

	float ReachHeight = Capsule->GetScaledCapsuleHalfHeight() * 0.55f;
	if (LeftHandController && RightHandController)
	{
		ReachHeight = FMath::Max(
			LeftHandController->GetRelativeLocation().Z,
			RightHandController->GetRelativeLocation().Z);
	}

	if (ReachHeight < Capsule->GetScaledCapsuleHalfHeight() * 0.55f)
	{
		return;
	}

	const FVector Forward = GetActorForwardVector();
	const FVector Start = GetActorLocation();
	const FVector End = Start + Forward * 90.f + FVector(0.f, 0.f, 130.f);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MistspireMantle), false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params) && Hit.bBlockingHit)
	{
		const FVector MantleDelta = FVector(0.f, 0.f, 95.f) + Forward * 45.f;
		AddActorWorldOffset(MantleDelta * FMath::Min(DeltaTime * 10.f, 1.f), true);
	}
}

bool AMistspireVRPawn::TryNonVRClimbMantle()
{
	UWorld* World = GetWorld();
	if (!World || !Capsule || !bNonVRMode)
	{
		return false;
	}

	const FVector FlatFwd = FVector(
		(VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector()).X,
		(VRCamera ? VRCamera->GetForwardVector() : GetActorForwardVector()).Y,
		0.f).GetSafeNormal();
	if (FlatFwd.IsNearlyZero())
	{
		return false;
	}

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	// Probe just ahead and above the feet for a walkable ledge top.
	const FVector LedgeProbeStart = GetActorLocation() + FlatFwd * 45.f + FVector(0.f, 0.f, HalfHeight * 0.35f);
	const FVector LedgeProbeEnd = LedgeProbeStart - FVector(0.f, 0.f, HalfHeight + 40.f);

	FHitResult LedgeHit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NonVRClimbMantle), false, this);
	const bool bHit = World->SweepSingleByChannel(
		LedgeHit, LedgeProbeStart, LedgeProbeEnd, FQuat::Identity, ECC_WorldStatic,
		FCollisionShape::MakeSphere(NonVRGroundProbeRadiusCm), Params);

	if (!bHit || !IsFloorHit(LedgeHit))
	{
		return false;
	}

	const float GapToLedge = (GetActorLocation().Z - HalfHeight) - LedgeHit.ImpactPoint.Z;
	// Only mantle when the ledge is near foot height or slightly below the capsule mid.
	if (GapToLedge < -NonVRMaxStepHeightCm || GapToLedge > HalfHeight * 0.85f)
	{
		return false;
	}

	SnapFeetToGround(LedgeHit);
	AddActorWorldOffset(FlatFwd * 40.f, true);

	FHitResult SettleHit;
	if (ProbeGround(SettleHit, NonVRMaxStepHeightCm))
	{
		SnapFeetToGround(SettleHit);
	}

	HorizontalVelocity = FVector::ZeroVector;
	VerticalVelocityCmPerSec = 0.f;
	return true;
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
	if (bGrappleActive || bGrappleExtending)
	{
		ReleaseGrapple();
		return;
	}

	if (!VRCamera)
	{
		return;
	}

	FVector Start;
	FVector End;
	if (bNonVRMode)
	{
		Start = VRCamera->GetComponentLocation();
		End = Start + VRCamera->GetForwardVector() * GrappleTraceDistanceCm;
	}
	else
	{
		if (!RightHandController)
		{
			return;
		}
		Start = GetRightHandWorldLocation();
		End = Start + VRCamera->GetForwardVector() * GrappleTraceDistanceCm;
	}

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MistspireGrapple), false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params) && Hit.bBlockingHit)
	{
		FireGrapple(Hit.ImpactPoint);
		if (!bNonVRMode)
		{
			if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
			{
				XR->TriggerHapticVibration(false, 0.6f, 0.1f, 120.f);
			}
		}
	}
}

void AMistspireVRPawn::PlaceNonVRGrappleCableStart()
{
	if (!bNonVRMode || !GrappleCable || !VRCamera)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	int32 SizeX = 0;
	int32 SizeY = 0;
	PC->GetViewportSize(SizeX, SizeY);
	if (SizeX <= 0 || SizeY <= 0)
	{
		GrappleCable->SetRelativeLocation(FVector(40.f, 45.f, -160.f));
		return;
	}

	// Deproject a point below the bottom edge (slightly right) so the cable root / end-cap is off-screen.
	FVector WorldPos, WorldDir;
	const float ScreenX = static_cast<float>(SizeX) * 0.62f;
	const float ScreenY = static_cast<float>(SizeY) * 1.12f;
	if (!PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldPos, WorldDir))
	{
		GrappleCable->SetRelativeLocation(FVector(40.f, 45.f, -160.f));
		return;
	}

	// Nudge further along the ray so the start sits past the near plane / bottom bezel.
	const FVector CamLoc = VRCamera->GetComponentLocation();
	const FVector Start = WorldPos + WorldDir * 25.f;
	const FVector ToStart = Start - CamLoc;
	// Keep a minimum distance below the camera so FOV changes still clip the tip.
	const FVector ClampedStart = CamLoc + ToStart.GetClampedToSize(80.f, 250.f);
	GrappleCable->SetWorldLocation(ClampedStart);
}

void AMistspireVRPawn::UpdateGrappleCableVisual(const FVector& CableEndWorld)
{
	if (GrappleAnchor)
	{
		GrappleAnchor->SetWorldLocation(CableEndWorld);
	}

	if (GrappleCable)
	{
		if (bNonVRMode)
		{
			PlaceNonVRGrappleCableStart();
		}

		const FVector Start = GrappleCable->GetComponentLocation();
		// Exact span (no slack) so the sim stays taut while pulling.
		GrappleCable->CableLength = FMath::Max(10.f, FVector::Dist(Start, CableEndWorld));
		GrappleCable->CableGravityScale = 0.f;
		GrappleCable->bEnableStiffness = true;
		GrappleCable->SetHiddenInGame(false);
	}
}

void AMistspireVRPawn::UpdateGrapple(float DeltaTime)
{
	const FVector CableStart = GrappleCable ? GrappleCable->GetComponentLocation() : GetActorLocation();

	if (bGrappleExtending)
	{
		const float Dist = FVector::Dist(CableStart, GrappleAnchorPoint);
		const float Step = Dist > KINDA_SMALL_NUMBER
			? (GrappleExtendSpeedCmPerSec * DeltaTime) / Dist
			: 1.f;
		GrappleExtendAlpha = FMath::Clamp(GrappleExtendAlpha + Step, 0.f, 1.f);
		const FVector Tip = FMath::Lerp(CableStart, GrappleAnchorPoint, GrappleExtendAlpha);
		UpdateGrappleCableVisual(Tip);

		if (GrappleExtendAlpha >= 1.f)
		{
			bGrappleExtending = false;
			UpdateGrappleCableVisual(GrappleAnchorPoint);
		}
		return;
	}

	UpdateGrappleCableVisual(GrappleAnchorPoint);

	float PullStrength = 1200.f;
	if (UWorld* World = GetWorld())
	{
		if (UMistspireXRActionSubsystem* XRInput = World->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			if (XRInput->GetInputState().bGrapplePressed)
			{
				PullStrength = 2200.f;
			}
		}
	}

	FVector ToAnchor = GrappleAnchorPoint - GetActorLocation();
	float Dist = ToAnchor.Size();
	if (Dist > 100.f)
	{
		FVector PullForce = ToAnchor.GetSafeNormal() * PullStrength * DeltaTime;
		FHitResult GrappleHit;
		AddActorWorldOffset(PullForce, true, &GrappleHit);

		// Slide along rock faces instead of jamming into them (common on uneven maps).
		if (GrappleHit.bBlockingHit)
		{
			const FVector Remaining = PullForce * (1.f - GrappleHit.Time);
			const FVector Slide = FVector::VectorPlaneProject(Remaining, GrappleHit.ImpactNormal);
			if (!Slide.IsNearlyZero(0.5f))
			{
				AddActorWorldOffset(Slide, true);
			}
		}

		if (GetLocalRole() < ROLE_Authority)
		{
			Server_ApplySmoothLocomotion(PullForce);
		}
	}
	else
	{
		ReleaseGrapple();
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
	if (BiomeWristText && GetWorld())
	{
		if (UMistspireEnvironmentSubsystem* Env = GetWorld()->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			const EMistspireBiomeType Biome = Env->GetCurrentBiome();
			FText BiomeName;
			FColor BiomeColor;
			switch (Biome)
			{
				case EMistspireBiomeType::Mist:    BiomeName = NSLOCTEXT("Mistspire", "BiomeMist", "MIST");    BiomeColor = FColor(100, 128, 153); break;
				case EMistspireBiomeType::Arid:    BiomeName = NSLOCTEXT("Mistspire", "BiomeArid", "ARID");    BiomeColor = FColor(179, 128,  51); break;
				case EMistspireBiomeType::Forest:  BiomeName = NSLOCTEXT("Mistspire", "BiomeForest", "FOREST");  BiomeColor = FColor( 26, 153,  51); break;
				case EMistspireBiomeType::Ember:   BiomeName = NSLOCTEXT("Mistspire", "BiomeEmber", "EMBER");   BiomeColor = FColor(204,  38,  13); break;
				case EMistspireBiomeType::Crystal: BiomeName = NSLOCTEXT("Mistspire", "BiomeCrystal", "CRYSTAL"); BiomeColor = FColor( 51, 204, 255); break;
			case EMistspireBiomeType::Void:    BiomeName = NSLOCTEXT("Mistspire", "BiomeVoid", "VOID");    BiomeColor = FColor( 13,  13,  38); break;
			case EMistspireBiomeType::Tundra:  BiomeName = NSLOCTEXT("Mistspire", "BiomeTundra", "TUNDRA");  BiomeColor = FColor(200, 210, 230); break;
			case EMistspireBiomeType::Aether:  BiomeName = NSLOCTEXT("Mistspire", "BiomeAether", "AETHER");  BiomeColor = FColor(153,  51, 255); break;
			case EMistspireBiomeType::Sanctum: BiomeName = NSLOCTEXT("Mistspire", "BiomeSanctum", "SANCTUM"); BiomeColor = FColor( 26,   0,  77); break;
			case EMistspireBiomeType::Pinnacle:BiomeName = NSLOCTEXT("Mistspire", "BiomePinnacle", "PINNACLE");BiomeColor = FColor(255, 242, 204); break;
			default:                           BiomeName = FText::GetEmpty();                              BiomeColor = FColor::White; break;
			}
			BiomeWristText->SetText(BiomeName);
			BiomeWristText->SetTextRenderColor(BiomeColor);
		}
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

void AMistspireVRPawn::ApplyVerticalVelocity(float DeltaCm, FHitResult* OutHit)
{
	if (FMath::IsNearlyZero(DeltaCm))
	{
		return;
	}

	FHitResult Hit;
	AddActorWorldOffset(FVector(0.f, 0.f, DeltaCm), true, &Hit);
	if (OutHit)
	{
		*OutHit = Hit;
	}
}

bool AMistspireVRPawn::IsFloorHit(const FHitResult& Hit) const
{
	return Hit.bBlockingHit && FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector) > 0.5f;
}

bool AMistspireVRPawn::ProbeGround(FHitResult& OutHit, float ExtraDownCm) const
{
	const UWorld* World = GetWorld();
	if (!World || !Capsule)
	{
		return false;
	}

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const FVector Start = GetActorLocation() - FVector(0.f, 0.f, HalfHeight - NonVRGroundProbeRadiusCm);
	const FVector End = Start - FVector(0.f, 0.f, ExtraDownCm);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NonVRGroundProbe), false, this);
	const bool bHit = World->SweepSingleByChannel(
		OutHit, Start, End, FQuat::Identity, ECC_WorldStatic,
		FCollisionShape::MakeSphere(NonVRGroundProbeRadiusCm), Params);
	return bHit && IsFloorHit(OutHit);
}

void AMistspireVRPawn::ClearNonVRGroundCache()
{
	bNonVRHasSupportCache = false;
	bNonVRGroundedSticky = false;
	NonVRCachedSupportZ = 0.f;
}

void AMistspireVRPawn::SnapFeetToGround(const FHitResult& GroundHit)
{
	if (!Capsule)
	{
		return;
	}

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const FVector Loc = GetActorLocation();
	float SupportZ = GroundHit.ImpactPoint.Z;
	const bool bStationary = HorizontalVelocity.SizeSquared() < FMath::Square(NonVRStationarySpeedCmPerSec);
	const UWorld* World = GetWorld();
	const float Dt = World ? World->GetDeltaSeconds() : (1.f / 60.f);

	if (bNonVRHasSupportCache)
	{
		const float Diff = SupportZ - NonVRCachedSupportZ;
		if (bStationary)
		{
			// Standing on pebble fields: keep the last solid height unless we clearly stepped.
			if (FMath::Abs(Diff) < NonVRStationarySupportStickCm)
			{
				SupportZ = NonVRCachedSupportZ;
			}
			else
			{
				NonVRCachedSupportZ = SupportZ;
			}
		}
		else
		{
			// Walking: ease support height so debris does not pop the camera.
			NonVRCachedSupportZ = FMath::FInterpTo(NonVRCachedSupportZ, SupportZ, Dt, 10.f);
			SupportZ = NonVRCachedSupportZ;
		}
	}
	else
	{
		NonVRCachedSupportZ = SupportZ;
		bNonVRHasSupportCache = true;
	}

	bNonVRGroundedSticky = true;

	const float DesiredZ = SupportZ + HalfHeight + NonVRGroundSkinCm;
	const float DeltaZ = DesiredZ - Loc.Z;

	if (FMath::Abs(DeltaZ) < NonVRGroundSnapDeadzoneCm)
	{
		VerticalVelocityCmPerSec = 0.f;
		GliderVelocity.Z = FMath::Max(0.f, GliderVelocity.Z);
		return;
	}

	VerticalVelocityCmPerSec = 0.f;
	GliderVelocity.Z = FMath::Max(0.f, GliderVelocity.Z);

	FHitResult MoveHit;
	AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ), true, &MoveHit);
	if (MoveHit.bBlockingHit && !IsFloorHit(MoveHit) && DeltaZ > 0.f)
	{
		return;
	}
}

void AMistspireVRPawn::ApplyNonVRWalkDelta(const FVector& InDelta)
{
	FVector Delta = InDelta;
	Delta.Z = 0.f;
	if (Delta.IsNearlyZero())
	{
		return;
	}

	const bool bGrappling = bGrappleActive || bGrappleExtending;

	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);

	if (Hit.bBlockingHit)
	{
		const FVector Remaining = Delta * (1.f - Hit.Time);
		if (Remaining.SizeSquared() > 1.f)
		{
			const bool bAllowStep = !bGrappling && VerticalVelocityCmPerSec <= 10.f;
			if (!(bAllowStep && TryNonVRStepUp(Remaining)))
			{
				FVector WallNormal = Hit.ImpactNormal;
				WallNormal.Z = 0.f;
				if (WallNormal.Normalize())
				{
					FVector Slide = FVector::VectorPlaneProject(Remaining, WallNormal);
					Slide.Z = 0.f;
					if (!Slide.IsNearlyZero(0.5f))
					{
						FHitResult SlideHit;
						AddActorWorldOffset(Slide, true, &SlideHit);
						if (bAllowStep && SlideHit.bBlockingHit)
						{
							TryNonVRStepUp(Slide * (1.f - SlideHit.Time));
						}
					}
				}
			}
		}
	}

	if (!bGrappling)
	{
		// Contour-follow only while moving — standing still must not chase pebble hits.
		if (HorizontalVelocity.SizeSquared() >= FMath::Square(NonVRStationarySpeedCmPerSec))
		{
			FollowNonVRGroundAfterMove();
		}
	}
}

bool AMistspireVRPawn::TryNonVRStepUp(const FVector& RemainingHorizontal)
{
	if (!Capsule || RemainingHorizontal.SizeSquared() < 1.f)
	{
		return false;
	}

	const FVector SavedLoc = GetActorLocation();
	const FRotator SavedRot = GetActorRotation();
	const float StepUp = NonVRMaxStepHeightCm;

	FHitResult UpHit;
	AddActorWorldOffset(FVector(0.f, 0.f, StepUp), true, &UpHit);
	if (UpHit.bBlockingHit && UpHit.Time < 0.15f)
	{
		SetActorLocationAndRotation(SavedLoc, SavedRot, false, nullptr, ETeleportType::TeleportPhysics);
		return false;
	}

	FHitResult FwdHit;
	AddActorWorldOffset(RemainingHorizontal, true, &FwdHit);
	if (FwdHit.bBlockingHit && FwdHit.Time <= KINDA_SMALL_NUMBER)
	{
		SetActorLocationAndRotation(SavedLoc, SavedRot, false, nullptr, ETeleportType::TeleportPhysics);
		return false;
	}

	FHitResult DownHit;
	AddActorWorldOffset(FVector(0.f, 0.f, -(StepUp + 8.f)), true, &DownHit);

	FHitResult GroundHit;
	if (IsFloorHit(DownHit))
	{
		SnapFeetToGround(DownHit);
		return true;
	}
	if (ProbeGround(GroundHit, StepUp + 12.f))
	{
		SnapFeetToGround(GroundHit);
		return true;
	}

	SetActorLocationAndRotation(SavedLoc, SavedRot, false, nullptr, ETeleportType::TeleportPhysics);
	return false;
}

void AMistspireVRPawn::FollowNonVRGroundAfterMove()
{
	if (!Capsule)
	{
		return;
	}

	// Rising jump or a real fall: gravity / landing sweep owns Z.
	if (VerticalVelocityCmPerSec > 10.f || VerticalVelocityCmPerSec < NonVRFallFollowCancelCmPerSec)
	{
		return;
	}

	FHitResult GroundHit;
	if (!ProbeGround(GroundHit, NonVRMaxStepHeightCm))
	{
		return;
	}

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float DesiredZ = GroundHit.ImpactPoint.Z + HalfHeight + NonVRGroundSkinCm;
	const float DeltaZ = DesiredZ - GetActorLocation().Z;

	if (FMath::Abs(DeltaZ) < NonVRGroundSnapDeadzoneCm)
	{
		return;
	}

	if (DeltaZ < -0.1f && -DeltaZ <= NonVRMaxStepHeightCm)
	{
		SnapFeetToGround(GroundHit);
	}
	else if (DeltaZ > 0.1f && DeltaZ <= NonVRMaxStepHeightCm)
	{
		SnapFeetToGround(GroundHit);
	}
}

void AMistspireVRPawn::EnforceNonVRGroundConstraint(bool bSkipWhileReelingUpward)
{
	if (!bNonVRMode || !Capsule)
	{
		return;
	}

	FHitResult GroundHit;
	if (!ProbeGround(GroundHit, FMath::Max(NonVRGroundSnapMaxGapCm, 400.f)))
	{
		return;
	}

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float GapToGround = (GetActorLocation().Z - HalfHeight) - GroundHit.ImpactPoint.Z;

	if (GapToGround < -NonVRGroundSkinCm)
	{
		SnapFeetToGround(GroundHit);
		return;
	}

	if (bSkipWhileReelingUpward)
	{
		return;
	}

	if (GapToGround <= NonVRGroundContactGapCm)
	{
		SnapFeetToGround(GroundHit);
	}
}

void AMistspireVRPawn::UpdateNonVRGravity(float DeltaTime)
{
	if (VerticalVelocityCmPerSec > 10.f)
	{
		ClearNonVRGroundCache();
	}

	const bool bStationary = HorizontalVelocity.SizeSquared() < FMath::Square(NonVRStationarySpeedCmPerSec);

	// Already planted and not moving: hold Z — do not re-probe pebbles every tick.
	if (bStationary && bNonVRGroundedSticky && bNonVRHasSupportCache && VerticalVelocityCmPerSec <= 0.f)
	{
		const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		const float GapCached = (GetActorLocation().Z - HalfHeight) - NonVRCachedSupportZ;
		if (GapCached >= -NonVRGroundSkinCm && GapCached <= NonVRGroundContactGapCm + 3.f)
		{
			VerticalVelocityCmPerSec = 0.f;
			return;
		}
	}

	FHitResult GroundHit;
	if (ProbeGround(GroundHit, NonVRGroundSnapMaxGapCm))
	{
		const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		const float GapToGround = (GetActorLocation().Z - HalfHeight) - GroundHit.ImpactPoint.Z;

		if (GapToGround < -NonVRGroundSkinCm)
		{
			SnapFeetToGround(GroundHit);
			return;
		}

		if (VerticalVelocityCmPerSec <= 0.f && GapToGround <= NonVRGroundContactGapCm)
		{
			if (bStationary && bNonVRGroundedSticky)
			{
				VerticalVelocityCmPerSec = 0.f;
				return;
			}
			SnapFeetToGround(GroundHit);
			return;
		}
	}
	else if (VerticalVelocityCmPerSec <= 0.f)
	{
		// Lost nearby support while not rising — clear sticky so IsGrounded is honest.
		ClearNonVRGroundCache();
	}

	VerticalVelocityCmPerSec -= NonVRGravityCmPerSec2 * DeltaTime;

	FHitResult MoveHit;
	ApplyVerticalVelocity(VerticalVelocityCmPerSec * DeltaTime, &MoveHit);

	if (VerticalVelocityCmPerSec <= 0.f && IsFloorHit(MoveHit))
	{
		SnapFeetToGround(MoveHit);
	}
}

void AMistspireVRPawn::UpdateNonVRCameraBob(float DeltaTime)
{
	if (!VRCamera || !bNonVRMode)
	{
		return;
	}

	float BobScale = 1.f;
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		if (!Settings->IsViewBobbingEnabled())
		{
			VRCamera->SetRelativeLocation(FVector(0.f, 0.f, NonVREyeHeightCm));
			NonVRHeadBobWeight = 0.f;
			return;
		}
		BobScale = Settings->GetViewBobScale();
	}

	const float Speed2D = HorizontalVelocity.Size();
	const float WalkRef = FMath::Max(1.f, DefaultLocomotionSpeedCmPerSec);
	const bool bBobbing = !bIsClimbing && !bGliderActive && !bGrappleActive
		&& IsGrounded() && Speed2D > NonVRStationarySpeedCmPerSec;

	float TargetWeight = 0.f;
	if (bBobbing)
	{
		TargetWeight = FMath::Clamp(Speed2D / WalkRef, 0.f, 1.35f);
		NonVRHeadBobPhase += DeltaTime * NonVRHeadBobStrideHz * TargetWeight * (2.f * PI);
		if (NonVRHeadBobPhase > 2.f * PI * 64.f)
		{
			NonVRHeadBobPhase = FMath::Fmod(NonVRHeadBobPhase, 2.f * PI);
		}
	}

	NonVRHeadBobWeight = FMath::FInterpTo(NonVRHeadBobWeight, TargetWeight, DeltaTime, bBobbing ? 8.f : 10.f);

	float BobY = 0.f;
	float BobZ = 0.f;
	if (NonVRHeadBobWeight > 0.01f)
	{
		BobZ = FMath::Sin(NonVRHeadBobPhase * 2.f) * NonVRHeadBobVerticalCm * NonVRHeadBobWeight * BobScale;
		BobY = FMath::Sin(NonVRHeadBobPhase) * NonVRHeadBobLateralCm * NonVRHeadBobWeight * BobScale;
	}

	VRCamera->SetRelativeLocation(FVector(0.f, BobY, NonVREyeHeightCm + BobZ));
}

bool AMistspireVRPawn::IsGrounded() const
{
	if (bNonVRMode)
	{
		return bNonVRGroundedSticky && VerticalVelocityCmPerSec <= 50.f;
	}

	FHitResult Hit;
	if (!ProbeGround(Hit, NonVRGroundContactGapCm))
	{
		return false;
	}

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float GapToGround = (GetActorLocation().Z - HalfHeight) - Hit.ImpactPoint.Z;
	return GapToGround <= NonVRGroundContactGapCm;
}

void AMistspireVRPawn::ApplyTeleport(const FVector& TargetLocation)
{
	HorizontalVelocity = FVector::ZeroVector;
	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ApplyTeleport(TargetLocation);
	}
}

bool AMistspireVRPawn::Server_ApplyTeleport_Validate(const FVector& TargetLocation)
{
	return FVector::DistSquared(GetActorLocation(), TargetLocation) <= FMath::Square(MaxTeleportRpcDistanceCm);
}
void AMistspireVRPawn::Server_ApplyTeleport_Implementation(const FVector& TargetLocation)
{
	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AMistspireVRPawn::TeleportForward(float DistanceCm)
{
	const float ClampedDistance = FMath::Clamp(DistanceCm, 0.f, TeleportForwardCm);
	const FVector Target = GetActorLocation() + VRCamera->GetForwardVector() * ClampedDistance;
	ApplyTeleport(Target);
}

void AMistspireVRPawn::ReceiveLoreShardLocal(const FText& Title, const FText& Body)
{
	if (UMistspireNarrativeSubsystem* Narr = GetWorld()->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(FText::Format(
			NSLOCTEXT("Mistspire", "LoreShard", "{0} — {1}"),
			Title, Body), 8.f);
	}

	if (!bNonVRMode)
	{
		if (UMistspireXRActionSubsystem* XR = GetWorld()->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			XR->TriggerHapticVibration(true, 0.2f, 0.08f, 110.f);
		}
	}
}

void AMistspireVRPawn::DeliverLoreShard(const FText& Title, const FText& Body)
{
	if (IsLocallyControlled())
	{
		ReceiveLoreShardLocal(Title, Body);
	}
	else if (GetNetMode() != NM_Client)
	{
		ClientReceiveLoreShard(Title, Body);
	}
}

void AMistspireVRPawn::ClientReceiveLoreShard_Implementation(const FText& Title, const FText& Body)
{
	ReceiveLoreShardLocal(Title, Body);
}

bool AMistspireVRPawn::Server_CollectLoreShard_Validate(AMistspireLoreShard* Shard)
{
	return Shard != nullptr;
}

void AMistspireVRPawn::Server_CollectLoreShard_Implementation(AMistspireLoreShard* Shard)
{
	if (!Shard || Shard->IsCollected())
	{
		return;
	}
	Shard->ApplyCollection(this);
}

void AMistspireVRPawn::StartClimb()
{
	bIsClimbing = true;
	NonVRClimbMissFrames = 0;
	HorizontalVelocity = FVector::ZeroVector;
	VerticalVelocityCmPerSec = 0.f;
	ClearNonVRGroundCache();
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
	NonVRClimbMissFrames = 0;
	LocomotionSpeedCmPerSec = bGliderActive ? DefaultLocomotionSpeedCmPerSec * 1.5f : DefaultLocomotionSpeedCmPerSec;

	// If we stopped on solid ground (crested a boulder), plant feet once.
	if (bNonVRMode)
	{
		FHitResult GroundHit;
		if (ProbeGround(GroundHit, NonVRMaxStepHeightCm))
		{
			SnapFeetToGround(GroundHit);
		}
	}

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
	bGrappleExtending = true;
	GrappleExtendAlpha = 0.f;
	GrappleAnchorPoint = WorldTarget;
	ClearNonVRGroundCache();

	if (GrappleCable)
	{
		GrappleCable->SetAttachEndToComponent(GrappleAnchor);
		GrappleCable->EndLocation = FVector::ZeroVector;
		GrappleCable->SetHiddenInGame(false);
		UpdateGrappleCableVisual(GrappleCable->GetComponentLocation());
	}

	if (UMistspireAudioSubsystem* Audio = GetWorld()->GetSubsystem<UMistspireAudioSubsystem>())
	{
		Audio->PlaySpatialSoundAtLocation(TEXT("grapple_fire"), WorldTarget, 0.7f);
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

void AMistspireVRPawn::ReleaseGrapple()
{
	bGrappleActive = false;
	bGrappleExtending = false;
	GrappleExtendAlpha = 0.f;

	if (GrappleCable)
	{
		GrappleCable->SetHiddenInGame(true);
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_ReleaseGrapple();
	}
}

bool AMistspireVRPawn::Server_ReleaseGrapple_Validate() { return true; }
void AMistspireVRPawn::Server_ReleaseGrapple_Implementation()
{
	ReleaseGrapple();
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
		GliderVelocity = HorizontalVelocity;
		GliderVelocity.Z = VerticalVelocityCmPerSec;
		if (GliderVelocity.SizeSquared2D() < 100.f)
		{
			GliderVelocity += GetActorForwardVector() * LocomotionSpeedCmPerSec;
		}
		HorizontalVelocity = FVector::ZeroVector;
	}
	else
	{
		VerticalVelocityCmPerSec = GliderVelocity.Z;
		HorizontalVelocity = FVector(GliderVelocity.X, GliderVelocity.Y, 0.f);
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
	if (bNonVRMode)
	{
		if (!bGameplayStarted || bSettingsMenuOpen || !IsGrounded() || VerticalVelocityCmPerSec > 0.f)
		{
			return;
		}
		VerticalVelocityCmPerSec = NonVRJumpImpulseCmPerSec;
		ClearNonVRGroundCache();
	}
	else
	{
		VerticalVelocityCmPerSec = JumpImpulseCmPerSec;
	}

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
