#include "MistspirePhysicalButton.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

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

#include "MistspireInteractionSubsystem.h"

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
	PhysicsConstraint->SetLinearZDrive(true);
}

void AMistspirePhysicalButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentOffset = (ButtonMesh->GetRelativeLocation() - InitialLocalLocation).Size();

	if (CurrentOffset > ActivationThresholdCm && !bIsPressed)
	{
		bIsPressed = true;
		OnButtonPressed.Broadcast();
	}
	else if (CurrentOffset < ActivationThresholdCm * 0.5f && bIsPressed)
	{
		bIsPressed = false;
	}
}
