#include "MistspirePhysicalButton.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireInteractionSubsystem.h"
#include "MistspireInteractable.h"
#include "MistspireVRPawn.h"
#include "MistspireXRActionSubsystem.h"
#include "MistspireInputMode.h"
#include "MistspireNarrativeSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/GameplayStatics.h"

AMistspirePhysicalButton::AMistspirePhysicalButton()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(BaseMesh);
	ButtonMesh->SetSimulatePhysics(true);
	ButtonMesh->SetLinearDamping(5.0f);
	ButtonMesh->SetAngularDamping(5.0f);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(BaseMesh);
}

void AMistspirePhysicalButton::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UMistspireInteractionSubsystem* Sub = World->GetSubsystem<UMistspireInteractionSubsystem>())
		{
			Sub->RegisterInteractiveActor(this);
		}
	}

	InitialLocalLocation = ButtonMesh->GetRelativeLocation();

	// Configure physics constraint for linear movement on Z axis only
	PhysicsConstraint->SetConstrainedComponents(BaseMesh, NAME_None, ButtonMesh, NAME_None);
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 5.0f); // 5cm travel
	
	// Spring to return button
	PhysicsConstraint->SetLinearDriveParams(500.0f, 50.0f, 0.0f);
	PhysicsConstraint->SetLinearPositionDrive(false, false, true);
}

void AMistspirePhysicalButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentOffset = (ButtonMesh->GetRelativeLocation() - InitialLocalLocation).Size();

	if (CurrentOffset > ActivationThresholdCm && !bIsPressed)
	{
		bIsPressed = true;
		OnButtonPressed.Broadcast();
		ExecuteBuiltInAction();
	}
	else if (CurrentOffset < ActivationThresholdCm * 0.5f && bIsPressed)
	{
		bIsPressed = false;
	}
}

void AMistspirePhysicalButton::ExecuteBuiltInAction()
{
	UWorld* World = GetWorld();
	if (!World || BuiltInAction == EMistspireButtonAction::None)
	{
		return;
	}

	if (!FMistspireInputMode::IsNonVRMode(World))
	{
		if (UMistspireXRActionSubsystem* XR = World->GetSubsystem<UMistspireXRActionSubsystem>())
		{
			XR->TriggerHapticVibration(true, 0.4f, 0.1f, 100.f);
		}
	}

	switch (BuiltInAction)
	{
		case EMistspireButtonAction::CycleWeather:
			if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
			{
				const int32 Next = (static_cast<int32>(Env->GetCurrentWeather()) + 1) % 4;
				Env->ForceWeather(static_cast<EMistspireWeatherType>(Next), 90.f);
				if (UMistspireNarrativeSubsystem* Narr = World->GetSubsystem<UMistspireNarrativeSubsystem>())
				{
					Narr->PushLine(FText::Format(
						NSLOCTEXT("Mistspire", "WeatherCycle", "Sky shifts: {0}"),
						Env->GetWeatherDisplayName()), 4.f);
				}
			}
			break;
		case EMistspireButtonAction::RefillSurvival:
			if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
			{
				if (AMistspireVRPawn* VR = Cast<AMistspireVRPawn>(Pawn))
				{
					VR->ApplyShelterRefill(50.f, 50.f, 1.f);
				}
			}
			break;
		case EMistspireButtonAction::TeleportUp:
			if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
			{
				Pawn->AddActorWorldOffset(FVector(0.f, 0.f, TeleportUpCm), false, nullptr, ETeleportType::TeleportPhysics);
			}
			break;
		default:
			break;
	}
}

void AMistspirePhysicalButton::MistspireInteract_Implementation(AActor* Instigator)
{
	if (!bIsPressed)
	{
		bIsPressed = true;
		OnButtonPressed.Broadcast();
		ExecuteBuiltInAction();
	}
}
