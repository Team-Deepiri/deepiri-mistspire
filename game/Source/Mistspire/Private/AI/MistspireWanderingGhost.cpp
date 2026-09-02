#include "AI/MistspireWanderingGhost.h"
#include "Components/StaticMeshComponent.h"
#include "AI/MistspireSteering.h"
#include "AI/MistspireStateMachine.h"
#include "MistspireEntitySubsystem.h"
#include "MistspireDialogueSubsystem.h"
#include "AI/MistspireAITypes.h"
#include "UObject/ConstructorHelpers.h"

AMistspireWanderingGhost::AMistspireWanderingGhost()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	SetRootComponent(GhostMesh);
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMesh->SetCastShadow(false);

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh.Succeeded())
		{
			GhostMesh->SetStaticMesh(SphereMesh.Object);
			GhostMesh->SetWorldScale3D(FVector(0.35f));
		}
	}

	Steering = CreateDefaultSubobject<UMistspireSteeringComponent>(TEXT("Steering"));
	Steering->MaxSpeedCmPerSec = 380.f;
	Steering->MaxForce = 900.f;
	Steering->SeparationRadiusCm = 300.f;

	StateMachine = CreateDefaultSubobject<UMistspireStateMachineComponent>(TEXT("StateMachine"));
}

void AMistspireWanderingGhost::BeginPlay()
{
	Super::BeginPlay();

	// State chart: Idle -> Wander -> Seek -> Idle ...
	TArray<FMistspireStateDefinition> States;
	{
		FMistspireStateDefinition Idle;
		Idle.StateId = TEXT("Idle");
		Idle.ValidInputs = { TEXT("wander") };
		States.Add(Idle);

		FMistspireStateDefinition Wandering;
		Wandering.StateId = TEXT("Wandering");
		Wandering.ValidInputs = { TEXT("seek"), TEXT("idle") };
		States.Add(Wandering);

		FMistspireStateDefinition Seeking;
		Seeking.StateId = TEXT("Seeking");
		Seeking.ValidInputs = { TEXT("idle") };
		States.Add(Seeking);
	}
	StateMachine->SetStates(States, TEXT("Idle"));
	StateMachine->OnStateChanged.AddDynamic(this, &AMistspireWanderingGhost::OnGhostStateChanged);

	// Register in the entity store so the observation recorder / other systems can query ghosts.
	if (UMistspireEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMistspireEntitySubsystem>())
	{
		EntityId = Entities->SpawnEntity(EntityArchetype, GetActorLocation(), this);
		if (EntityId != INDEX_NONE)
		{
			Entities->SetEntityFloat(EntityId, TEXT("AltitudeCm"), GetActorLocation().Z);
		}
	}

	PickNextBehavior();
}

void AMistspireWanderingGhost::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMistspireEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMistspireEntitySubsystem>())
	{
		if (EntityId != INDEX_NONE)
		{
			Entities->DestroyEntity(EntityId);
			EntityId = INDEX_NONE;
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AMistspireWanderingGhost::PickNextBehavior()
{
	FName NextInput = TEXT("wander");
	switch (FMath::RandRange(0, 2))
	{
		case 0:
			NextInput = TEXT("wander");
			break;
		case 1:
			NextInput = TEXT("seek");
			break;
		default:
			NextInput = TEXT("idle");
			break;
	}
	StateMachine->PushInput(NextInput);
	BehaviorTimer = 0.f;
}

void AMistspireWanderingGhost::OnGhostStateChanged(FName OldState, FName NewState)
{
	if (!Steering)
	{
		return;
	}

	if (NewState == TEXT("Wandering"))
	{
		Steering->SetMode(EMistspireSteeringMode::Wander);
		Steering->SetTarget(GetActorLocation());
	}
	else if (NewState == TEXT("Seeking"))
	{
		// Seek the player or, failing that, stay high.
		if (APawn* Player = GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr)
		{
			Steering->SetMode(EMistspireSteeringMode::Arrive);
			Steering->SetTarget(Player->GetActorLocation() + FVector(0.f, 0.f, 300.f));
		}
		else
		{
			Steering->SetMode(EMistspireSteeringMode::Seek);
			Steering->SetTarget(GetActorLocation() + FVector(0.f, 0.f, 2000.f));
		}
	}
	else
	{
		Steering->SetMode(EMistspireSteeringMode::Seek);
		Steering->SetTarget(GetActorLocation());
	}

	if (NewState == TEXT("Wandering"))
	{
		if (UMistspireDialogueSubsystem* Dialogue = GetWorld()->GetSubsystem<UMistspireDialogueSubsystem>())
		{
			Dialogue->Speak(TEXT("ghost_whisper"));
		}
	}
}

void AMistspireWanderingGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StateMachine)
	{
		BehaviorTimer += DeltaTime;
		if (BehaviorTimer >= BehaviorChangeIntervalSeconds)
		{
			PickNextBehavior();
		}
	}

	if (!Steering)
	{
		return;
	}

	const FVector Velocity = Steering->ComputeDesiredVelocity(
		GetActorLocation(), GetVelocity(), DeltaTime);
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);

	if (UMistspireEntitySubsystem* Entities = GetWorld()->GetSubsystem<UMistspireEntitySubsystem>())
	{
		if (EntityId != INDEX_NONE)
		{
			Entities->SetEntityFloat(EntityId, TEXT("AltitudeCm"), GetActorLocation().Z);
		}
	}
}