#include "MistspireDemoClimbScaffold.h"
#include "MistspireDemoSpireLayout.h"
#include "MistspireSummitMarker.h"
#include "MistspireRestShelter.h"
#include "MistspireOxygenCanister.h"
#include "MistspireWindCrystal.h"
#include "MistspireLoreShard.h"
#include "MistspirePhysicalButton.h"
#include "MistspireBuildingEntrance.h"
#include "MistspireInteriorExit.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireNarrativeSubsystem.h"
#include "MistspireVRPawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AMistspireDemoClimbScaffold::AMistspireDemoClimbScaffold()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (CubeFinder.Succeeded())
	{
		CubeMesh = CubeFinder.Object;
	}
	if (CylFinder.Succeeded())
	{
		CylinderMesh = CylFinder.Object;
	}
	if (MatFinder.Succeeded())
	{
		BaseMaterial = MatFinder.Object;
	}
}

void AMistspireDemoClimbScaffold::BeginPlay()
{
	Super::BeginPlay();
	Rebuild();
}

AMistspireDemoClimbScaffold* AMistspireDemoClimbScaffold::EnsureInWorld(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	const bool bPlayWorld = World->WorldType == EWorldType::Game
		|| World->WorldType == EWorldType::PIE
		|| World->WorldType == EWorldType::GamePreview;
	if (!bPlayWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire DemoClimbScaffold: refused — not a play world (avoids dirtying Main_WP)."));
		return nullptr;
	}

	for (TActorIterator<AMistspireDemoClimbScaffold> It(World); It; ++It)
	{
		AMistspireDemoClimbScaffold* Existing = *It;
		if (Existing && !Existing->IsActorBeingDestroyed())
		{
			Existing->SetActorLocation(MistspireDemoSpire::GetValleyOrigin());
			Existing->Rebuild();
			return Existing;
		}
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	return World->SpawnActor<AMistspireDemoClimbScaffold>(
		AMistspireDemoClimbScaffold::StaticClass(),
		MistspireDemoSpire::GetValleyOrigin(),
		FRotator::ZeroRotator,
		Params);
}

void AMistspireDemoClimbScaffold::Rebuild()
{
	ClearBuiltActors();
	CachedTintMIDs.Reset();
	bEnvDressResolved = false;
	bLegacyDressPurged = false;

	if (!CubeMesh)
	{
		CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	if (!CylinderMesh)
	{
		CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	}
	if (!BaseMaterial)
	{
		BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	if (bSpawnEnvDress)
	{
		ResolveEnvDressMeshes();
	}

	PurgeLegacyMapDress();
	HideTemplateLandscape();
	AimSunAtVillage();
	BuildValley();
	BuildMistInnPocket();
	BuildCentralMast();
	if (bSpawnApproachHelix)
	{
		BuildApproachHelix();
		BuildGrappleShaftToMist();
	}

	for (int32 i = 0; i < MistspireDemoSpire::StationCount; ++i)
	{
		BuildStation(i);
	}

	if (bSpawnImmersionProps)
	{
		SpawnValleyImmersionProps();
	}

	StartFallCatch();

	UE_LOG(LogTemp, Log,
		TEXT("Mistspire DemoClimbScaffold: rebuilt %d stations + valley/Mist Inn/approach/shaft (EnvDress rocks=%d mtns=%d)."),
		MistspireDemoSpire::StationCount,
		EnvRocks.Num(),
		EnvMountains.Num());
}

void AMistspireDemoClimbScaffold::ClearBuiltActors()
{
	if (Root)
	{
		TArray<USceneComponent*> ChildComps;
		Root->GetChildrenComponents(true, ChildComps);
		for (USceneComponent* Child : ChildComps)
		{
			if (Child && Child != Root)
			{
				const FName Unique = MakeUniqueObjectName(this, Child->GetClass(), TEXT("DemoMesh_Retired"));
				Child->Rename(*Unique.ToString(), nullptr, REN_DoNotDirty);
				Child->DestroyComponent();
			}
		}
	}

	for (AActor* Prop : SpawnedPropActors)
	{
		if (IsValid(Prop))
		{
			Prop->Destroy();
		}
	}
	SpawnedPropActors.Reset();
}

UMaterialInstanceDynamic* AMistspireDemoClimbScaffold::GetOrCreateTintMID(const FLinearColor& Tint)
{
	if (!BaseMaterial)
	{
		return nullptr;
	}

	for (UMaterialInstanceDynamic* Existing : CachedTintMIDs)
	{
		if (!Existing)
		{
			continue;
		}
		FLinearColor Current;
		if (Existing->GetVectorParameterValue(TEXT("Color"), Current) && Current.Equals(Tint, 0.01f))
		{
			return Existing;
		}
	}

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	if (MID)
	{
		MID->SetVectorParameterValue(TEXT("Color"), Tint);
		MID->SetVectorParameterValue(TEXT("BaseColor"), Tint);
		CachedTintMIDs.Add(MID);
	}
	return MID;
}

UStaticMeshComponent* AMistspireDemoClimbScaffold::AddCube(
	const FName& NameBase,
	const FVector& WorldLocation,
	const FVector& Scale100cm,
	const FRotator& Rotation,
	const FLinearColor& Tint,
	bool bCollision)
{
	if (!CubeMesh || !Root)
	{
		return nullptr;
	}

	const FName UniqueName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), NameBase);
	UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(this, UniqueName);
	Comp->SetupAttachment(Root);
	Comp->SetMobility(EComponentMobility::Movable);
	Comp->SetStaticMesh(CubeMesh);
	Comp->SetWorldLocation(WorldLocation);
	Comp->SetWorldRotation(Rotation);
	Comp->SetWorldScale3D(Scale100cm);
	if (UMaterialInstanceDynamic* MID = GetOrCreateTintMID(Tint))
	{
		Comp->SetMaterial(0, MID);
	}
	if (bCollision)
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Comp->SetCollisionProfileName(TEXT("BlockAll"));
	}
	else
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Comp->RegisterComponent();
	return Comp;
}

UStaticMeshComponent* AMistspireDemoClimbScaffold::AddCylinder(
	const FName& NameBase,
	const FVector& WorldLocation,
	const FVector& Scale100cm,
	const FRotator& Rotation,
	const FLinearColor& Tint,
	bool bCollision)
{
	if (!CylinderMesh || !Root)
	{
		return nullptr;
	}

	const FName UniqueName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), NameBase);
	UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(this, UniqueName);
	Comp->SetupAttachment(Root);
	Comp->SetMobility(EComponentMobility::Movable);
	Comp->SetStaticMesh(CylinderMesh);
	Comp->SetWorldLocation(WorldLocation);
	Comp->SetWorldRotation(Rotation);
	Comp->SetWorldScale3D(Scale100cm);
	if (UMaterialInstanceDynamic* MID = GetOrCreateTintMID(Tint))
	{
		Comp->SetMaterial(0, MID);
	}
	if (bCollision)
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Comp->SetCollisionProfileName(TEXT("BlockAll"));
	}
	else
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Comp->RegisterComponent();
	return Comp;
}

void AMistspireDemoClimbScaffold::ResolveEnvDressMeshes()
{
	if (bEnvDressResolved)
	{
		return;
	}
	bEnvDressResolved = true;

	EnvRocks.Reset();
	EnvMountains.Reset();

	auto SoftLoad = [](const TCHAR* Path) -> UStaticMesh*
	{
		return LoadObject<UStaticMesh>(nullptr, Path);
	};

	static const TCHAR* RockPaths[] = {
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_01/StaticMeshes/SM_Rock_01.SM_Rock_01"),
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_02/StaticMeshes/SM_Rock_02.SM_Rock_02"),
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_03/StaticMeshes/SM_Rock_03.SM_Rock_03"),
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_04/StaticMeshes/SM_Rock_04.SM_Rock_04"),
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_05/StaticMeshes/SM_Rock_05.SM_Rock_05"),
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_06/StaticMeshes/SM_Rock_06.SM_Rock_06"),
		TEXT("/Game/Rock_Collection_04/Meshes/Rock_07/StaticMeshes/SM_Rock_07.SM_Rock_07"),
	};
	for (const TCHAR* Path : RockPaths)
	{
		if (UStaticMesh* Mesh = SoftLoad(Path))
		{
			EnvRocks.Add(Mesh);
		}
	}

	static const TCHAR* MountainPaths[] = {
		TEXT("/Game/Iceland_Environment/Static_Meshes/SM_Mountain_01.SM_Mountain_01"),
		TEXT("/Game/Iceland_Environment/Static_Meshes/SM_Mountain_03.SM_Mountain_03"),
		TEXT("/Game/Iceland_Environment/Static_Meshes/SM_Mountain_05.SM_Mountain_05"),
		TEXT("/Game/Iceland_Environment/Static_Meshes/SM_Iceland_Crest.SM_Iceland_Crest"),
		TEXT("/Game/Iceland_Environment/Static_Meshes/SM_Iceland_Eroded_Mountain.SM_Iceland_Eroded_Mountain"),
		TEXT("/Game/Iceland_Environment/Static_Meshes/SM_Mountain_Plateu_01.SM_Mountain_Plateu_01"),
	};
	for (const TCHAR* Path : MountainPaths)
	{
		if (UStaticMesh* Mesh = SoftLoad(Path))
		{
			EnvMountains.Add(Mesh);
		}
	}

	EnvPine = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Foliage/SM_Pine_Tree_01.SM_Pine_Tree_01"));
	EnvGrass = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Foliage/Grass_Patch_1.Grass_Patch_1"));
	EnvPorch = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Modular/Porch_4x4m.Porch_4x4m"));
	EnvDoor = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Modular/Door_01.Door_01"));
	EnvWall = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Modular/Wall_4m.Wall_4m"));
	// Prefer the wall that already has a door opening — solid Wall_4m + floating Door_01 was the blank facade.
	EnvWallDoor = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Modular/Wall_Door_4m.Wall_Door_4m"));
	EnvRoof = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Modular/Roof_Both_Ends_4m.Roof_Both_Ends_4m"));
	if (!EnvRoof)
	{
		EnvRoof = SoftLoad(TEXT("/Game/Modular_Rural_Cabin/Meshes/Modular/Roof_4m.Roof_4m"));
	}
}

UStaticMeshComponent* AMistspireDemoClimbScaffold::AddEnvMesh(
	const FName& NameBase,
	UStaticMesh* Mesh,
	const FVector& WorldLocation,
	const FVector& Scale,
	const FRotator& Rotation,
	bool bCollision)
{
	if (!Mesh || !Root)
	{
		return nullptr;
	}

	const FName UniqueName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), NameBase);
	UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(this, UniqueName);
	Comp->SetupAttachment(Root);
	Comp->SetMobility(EComponentMobility::Movable);
	Comp->SetStaticMesh(Mesh);
	Comp->SetWorldLocation(WorldLocation);
	Comp->SetWorldRotation(Rotation);
	Comp->SetWorldScale3D(Scale);
	if (bCollision)
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Comp->SetCollisionProfileName(TEXT("BlockAll"));
	}
	else
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Comp->RegisterComponent();
	return Comp;
}

void AMistspireDemoClimbScaffold::AimSunAtVillage()
{
	UWorld* World = GetWorld();
	if (!bAimSunAtVillage || !World)
	{
		return;
	}

	// The template sun left the shelf in the mountain's own shadow. Light now travels roughly
	// -X and down, so it comes from over the spawn side (+X) and rakes the face the village
	// sits on — and stays behind the player, who spawns looking -X toward the gate.
	const FRotator SunRotation(-38.f, 162.f, 0.f);
	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		ADirectionalLight* Sun = *It;
		if (!Sun || Sun->IsActorBeingDestroyed())
		{
			continue;
		}
		if (UDirectionalLightComponent* SunComp = Cast<UDirectionalLightComponent>(Sun->GetLightComponent()))
		{
			// Template suns are often Stationary, which silently ignores runtime rotation.
			SunComp->SetMobility(EComponentMobility::Movable);
			SunComp->SetWorldRotation(SunRotation);
		}
	}
}

namespace
{
	bool IsFabEnvStaticMesh(const UStaticMesh* Mesh)
	{
		if (!Mesh)
		{
			return false;
		}
		const FString Path = Mesh->GetPathName();
		return Path.Contains(TEXT("/Iceland_Environment/"), ESearchCase::IgnoreCase)
			|| Path.Contains(TEXT("/Rock_Collection_04/"), ESearchCase::IgnoreCase);
	}

	bool ActorUsesFabEnvMesh(const AActor* Actor)
	{
		if (!Actor)
		{
			return false;
		}
		TArray<UStaticMeshComponent*> MeshComps;
		Actor->GetComponents<UStaticMeshComponent>(MeshComps);
		for (const UStaticMeshComponent* Comp : MeshComps)
		{
			if (Comp && IsFabEnvStaticMesh(Comp->GetStaticMesh()))
			{
				return true;
			}
		}
		return false;
	}
}

void AMistspireDemoClimbScaffold::PurgeLegacyMapDress()
{
	UWorld* World = GetWorld();
	if (!bPurgeLegacyMapDress || bLegacyDressPurged || !World)
	{
		return;
	}

	using namespace MistspireDemoSpire;
	const float MaxAltitude = LegacyDressPurgeMaxAltitudeCm;

	TArray<AActor*> ToDestroy;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsActorBeingDestroyed() || Actor == this)
		{
			continue;
		}
		if (Cast<AMistspireDemoClimbScaffold>(Actor))
		{
			continue;
		}

		const FVector Loc = Actor->GetActorLocation();
		const bool bDemoEnvName = Actor->GetName().StartsWith(TEXT("DemoEnv_"));
		const bool bLowAltitude = Loc.Z < MaxAltitude;
		const bool bFabEnv = ActorUsesFabEnvMesh(Actor);

		// All authored DemoEnv_* from DL_Landmarks_Authored — superseded by SummitMass + shelf rocks.
		// Low-altitude Iceland/Rock world actors are legacy map dress; runtime Fab env lives on the
		// scaffold as child components, not as separate StaticMeshActors.
		if (bDemoEnvName || (bFabEnv && bLowAltitude))
		{
			ToDestroy.Add(Actor);
		}
	}

	int32 PurgedCount = 0;
	for (AActor* Actor : ToDestroy)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
			++PurgedCount;
		}
	}

	bLegacyDressPurged = true;
	UE_LOG(LogTemp, Log,
		TEXT("Mistspire DemoClimbScaffold: purged %d legacy map dress actors (DemoEnv_* + distant Fab tiles)."),
		PurgedCount);
}

void AMistspireDemoClimbScaffold::HideTemplateLandscape()
{
	UWorld* World = GetWorld();
	if (!bHideTemplateLandscape || !World)
	{
		return;
	}

	// Main_WP was created from /Engine/Maps/Templates/OpenWorld, which ships a flat untextured
	// landscape. Hide it and disable collision — leaving collision on caused interior-exit
	// teleports to land on (or get ejected by) landscape instead of the demo ShelfPad.
	UClass* LandscapeProxyClass = FindObject<UClass>(nullptr, TEXT("/Script/Landscape.LandscapeProxy"));
	if (!LandscapeProxyClass)
	{
		return;
	}

	int32 HiddenCount = 0;
	for (TActorIterator<AActor> It(World, LandscapeProxyClass); It; ++It)
	{
		AActor* Proxy = *It;
		if (!Proxy || Proxy->IsActorBeingDestroyed())
		{
			continue;
		}
		Proxy->SetActorHiddenInGame(true);
		Proxy->SetActorEnableCollision(false);
		++HiddenCount;
	}

	if (HiddenCount != LastHiddenLandscapeCount)
	{
		LastHiddenLandscapeCount = HiddenCount;
		UE_LOG(LogTemp, Log, TEXT("Mistspire DemoClimbScaffold: hiding %d template landscape proxies."), HiddenCount);
	}

	// World Partition streams landscape cells in as the player climbs, so re-apply on a slow
	// tick rather than once at build time.
	if (!World->GetTimerManager().IsTimerActive(LandscapeHideTimer))
	{
		World->GetTimerManager().SetTimer(
			LandscapeHideTimer, this, &AMistspireDemoClimbScaffold::HideTemplateLandscape, 3.f, true);
	}
}

void AMistspireDemoClimbScaffold::StartFallCatch()
{
	UWorld* World = GetWorld();
	if (!bCatchFallenPlayer || !World)
	{
		return;
	}
	if (!World->GetTimerManager().IsTimerActive(FallCatchTimer))
	{
		World->GetTimerManager().SetTimer(
			FallCatchTimer, this, &AMistspireDemoClimbScaffold::CatchFallenPlayer, 0.4f, true);
	}
}

void AMistspireDemoClimbScaffold::CatchFallenPlayer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn)
	{
		return;
	}

	// Every piece of demo geometry — shelf, helix, shaft, stations, Mist Inn pocket — lives at
	// or above the valley floor, and the template landscape is collision-disabled. Anything
	// below this is an unrecoverable fall into empty space, which ends a recording take.
	const float VoidZ = MistspireDemoSpire::GetValleyFloorZCm() - FallCatchDepthCm;
	if (Pawn->GetActorLocation().Z >= VoidZ)
	{
		return;
	}

	const FVector Spawn = MistspireDemoSpire::GetValleySpawnLocation();
	if (AMistspireVRPawn* MistPawn = Cast<AMistspireVRPawn>(Pawn))
	{
		MistPawn->ResetMotionForDebugTeleport();
		MistPawn->ApplyTeleport(Spawn);
	}
	else
	{
		Pawn->SetActorLocation(Spawn, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (UMistspireNarrativeSubsystem* Narr = World->GetSubsystem<UMistspireNarrativeSubsystem>())
	{
		Narr->PushLine(NSLOCTEXT("Mistspire", "FallCatch", "The mist carries you back to the Gate."), 4.f);
	}

	UE_LOG(LogTemp, Log, TEXT("Mistspire DemoClimbScaffold: caught fall below Z=%.0f, returned to Valley Gate."), VoidZ);
}

UStaticMeshComponent* AMistspireDemoClimbScaffold::AddTerrainMassif(
	const FName& NameBase,
	UStaticMesh* Mesh,
	const FVector2D& OffsetFromValleyCm,
	float FootprintRadiusCm,
	float DesiredTopZCm,
	float YawDeg)
{
	using namespace MistspireDemoSpire;
	if (!Mesh)
	{
		return nullptr;
	}

	const FBoxSphereBounds Bounds = Mesh->GetBounds();
	const double MeshRadius = FMath::Max3(Bounds.BoxExtent.X, Bounds.BoxExtent.Y, 1.0);
	const float Scale = static_cast<float>(FootprintRadiusCm / MeshRadius);
	const float TopOffsetZ = static_cast<float>((Bounds.Origin.Z + Bounds.BoxExtent.Z) * Scale);

	const FVector Origin = GetValleyOrigin();
	const FVector Loc(
		Origin.X + OffsetFromValleyCm.X,
		Origin.Y + OffsetFromValleyCm.Y,
		DesiredTopZCm - TopOffsetZ);

	return AddEnvMesh(NameBase, Mesh, Loc, FVector(Scale), FRotator(0.f, YawDeg, 0.f), false);
}

void AMistspireDemoClimbScaffold::DressValleyEnv()
{
	using namespace MistspireDemoSpire;
	const FRotator Identity = FRotator::ZeroRotator;

	// ONE mountain under the village, 3 km across. Pinning it by bounds left the whole mass
	// below the deck, where the deck itself occluded it and the village read as floating over
	// void — so it is placed by tracing its own surface (below) instead.
	// NoCollision — ShelfPad + gate rocks own walk collision (avoids capsule traps).
	if (EnvMountains.Num() > 0)
	{
		// Prefer eroded mountain if present (index 4 in ResolveEnvDressMeshes list).
		UStaticMesh* MassMesh = EnvMountains[0];
		if (EnvMountains.Num() > 4 && EnvMountains[4])
		{
			MassMesh = EnvMountains[4]; // SM_Iceland_Eroded_Mountain
		}

		UStaticMeshComponent* Mass = AddTerrainMassif(
			TEXT("SummitMass"),
			MassMesh,
			FVector2D::ZeroVector,
			150000.f,                          // 1.5 km half-extent: reaches the valley wall ring
			GetValleyFloorZCm() + 30000.f,     // provisional; corrected by the surface trace
			25.f);

		if (Mass)
		{
			// Slide the mass vertically until its surface under the WHOLE village sits below the
			// deck. Aligning only at the origin left the gate flush against a rising slope behind
			// the mast — sample the pad, gate, spawn, and inn, then sink to the highest hit.
			Mass->SetCollisionProfileName(TEXT("BlockAll"));
			Mass->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			const FVector Origin = GetValleyOrigin();
			const float FloorZ = GetValleyFloorZCm();
			const FVector Gate = GetValleyGateLocation();
			const FVector Inn = GetMistInnDoorLocation();
			const FVector Samples[] = {
				Origin,
				Gate,
				Gate + FVector(0.f, -400.f, 0.f),
				Gate + FVector(0.f, 400.f, 0.f),
				GetValleySpawnLocation(),
				FVector(Inn.X, Inn.Y, FloorZ),
				Valley(-800.f, 0.f, 0.f),
				Valley(0.f, -800.f, 0.f),
				Valley(0.f, 800.f, 0.f),
			};

			FCollisionQueryParams TraceParams(FName(TEXT("MistspireSummitMass")), /*bTraceComplex*/ true);
			float HighestSurfaceZ = -TNumericLimits<float>::Max();
			bool bAnyHit = false;
			for (const FVector& Sample : Samples)
			{
				FHitResult Hit;
				const FVector TraceStart(Sample.X, Sample.Y, FloorZ + 300000.f);
				const FVector TraceEnd(Sample.X, Sample.Y, FloorZ - 600000.f);
				if (Mass->LineTraceComponent(Hit, TraceStart, TraceEnd, TraceParams))
				{
					HighestSurfaceZ = FMath::Max(HighestSurfaceZ, static_cast<float>(Hit.ImpactPoint.Z));
					bAnyHit = true;
				}
			}

			if (bAnyHit)
			{
				// Keep the mountain under the walk surface across the whole shelf footprint.
				const float TargetSurfaceZ = FloorZ - 80.f;
				Mass->AddWorldOffset(FVector(0.f, 0.f, TargetSurfaceZ - HighestSurfaceZ));
			}

			Mass->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// Shelf rocks. Anything solid is refused inside the inn doorway's clearance so dressing can
	// never wall the door off again, and the walk line from spawn to the inn is left open.
	const FVector Gate = GetValleyGateLocation();
	const FVector InnDoorXY = GetMistInnDoorLocation();
	auto PlaceShelfRock = [&](const TCHAR* Name, int32 MeshIdx, const FVector& Loc, float Scale, float YawDeg)
	{
		if (MeshIdx < 0 || MeshIdx >= EnvRocks.Num())
		{
			return;
		}
		if (FVector::Dist2D(Loc, InnDoorXY) < InnDoorClearanceCm)
		{
			UE_LOG(LogTemp, Warning, TEXT("Mistspire DemoClimbScaffold: skipped %s — inside Mist Inn door clearance."), Name);
			return;
		}
		AddEnvMesh(Name, EnvRocks[MeshIdx], Loc, FVector(Scale), FRotator(0.f, YawDeg, 0.f), true);
	};

	PlaceShelfRock(TEXT("GateRock_L"), 0, Gate + FVector(-80.f, -520.f, 0.f), 1.8f, 35.f);
	PlaceShelfRock(TEXT("GateRock_R"), 1, Gate + FVector(-80.f, 540.f, 0.f), 1.6f, -40.f);
	PlaceShelfRock(TEXT("ApproachRock"), 2, Valley(1100.f, 280.f, 0.f), 1.4f, 110.f);
	PlaceShelfRock(TEXT("ShelfRock_A"), 3, Valley(250.f, 950.f, 0.f), 1.2f, 60.f);
	PlaceShelfRock(TEXT("ShelfRock_B"), EnvRocks.Num() > 4 ? 4 : 3, Valley(-450.f, 1150.f, 0.f), 1.1f, -20.f);

	if (EnvGrass)
	{
		AddEnvMesh(TEXT("ShelfGrass_A"), EnvGrass, Valley(300.f, 880.f, 8.f), FVector(1.3f), Identity, false);
		AddEnvMesh(TEXT("ShelfGrass_B"), EnvGrass, Valley(520.f, -1050.f, 8.f), FVector(1.1f), FRotator(0.f, 70.f, 0.f), false);
	}

	// Deliberately no ring of secondary landforms or rim pines — they read as floating slices
	// outside SummitMass and cost draw calls on the recording path. Overlapping copies around SummitMass still
	// read as separate slabs rather than one range, and they add nothing once the village sits
	// on a single 3 km mountain — the summit and the sky are the whole backdrop.
}

void AMistspireDemoClimbScaffold::DressMistInnPorchEnv()
{
	using namespace MistspireDemoSpire;

	// Assemble the cabin from each mesh's own bounds. Front is Wall_Door_4m (has the opening).
	// Do NOT place Door_01 on top — that leaf's pivot does not match the cutout and was the
	// "door not in the frame" look. Cabin shell is visual-only (NoCollision); enter is the
	// atlas BuildingEntrance trigger on the plaza side of the wall.
	UStaticMesh* FrontWallMesh = EnvWallDoor ? EnvWallDoor.Get() : EnvWall.Get();
	UStaticMesh* SideWallMesh = EnvWall.Get();
	if (!FrontWallMesh || !SideWallMesh)
	{
		return;
	}

	const FBoxSphereBounds FrontB = FrontWallMesh->GetBounds();
	const FBoxSphereBounds SideB = SideWallMesh->GetBounds();

	const float ModuleWidth = 2.f * FMath::Max(static_cast<float>(SideB.BoxExtent.X), static_cast<float>(SideB.BoxExtent.Y));
	const float HalfModule = 0.5f * ModuleWidth;
	const float FrontBottomLocal = static_cast<float>(FrontB.Origin.Z - FrontB.BoxExtent.Z);
	const float FrontTopLocal = static_cast<float>(FrontB.Origin.Z + FrontB.BoxExtent.Z);

	const FVector InnDoor = GetMistInnDoorLocation();
	const FRotator FacePlaza(0.f, 90.f, 0.f);
	const FRotator FaceSide(0.f, 180.f, 0.f);
	const FVector Forward(0.f, 1.f, 0.f);
	const FVector Right(1.f, 0.f, 0.f);
	const float WallPivotZ = GetValleyFloorZCm() - FrontBottomLocal;

	// If the pack authored a door socket, shift the wall so that socket lands on InnDoor —
	// Wall_Door openings are often off-center in the 4m module.
	FVector FrontLoc(InnDoor.X, InnDoor.Y, WallPivotZ);
	FVector SocketLocal = FVector::ZeroVector;
	bool bHasDoorSocket = false;
	for (UStaticMeshSocket* Socket : FrontWallMesh->Sockets)
	{
		if (!Socket)
		{
			continue;
		}
		const FString NameStr = Socket->SocketName.ToString();
		if (NameStr.Contains(TEXT("Door"), ESearchCase::IgnoreCase)
			|| NameStr.Contains(TEXT("Entry"), ESearchCase::IgnoreCase))
		{
			SocketLocal = Socket->RelativeLocation;
			bHasDoorSocket = true;
			break;
		}
	}
	if (bHasDoorSocket)
	{
		// Socket is in mesh local space; FacePlaza maps local +X → world +Y, local +Y → world -X.
		const FVector SocketWorldOffset = FacePlaza.RotateVector(SocketLocal);
		FrontLoc = FVector(
			InnDoor.X - SocketWorldOffset.X,
			InnDoor.Y - SocketWorldOffset.Y,
			WallPivotZ);
	}

	// Visual shell only — convex collision on Wall_Door fills the opening and blocked enter.
	AddEnvMesh(TEXT("InnEnvWall_Front"), FrontWallMesh, FrontLoc, FVector(1.f), FacePlaza, false);

	// Door leaf in the cutout. Prefer the wall's door socket; otherwise seat the leaf on the
	// atlas door XY using the door mesh's own bottom offset (no guessed cm).
	if (EnvDoor)
	{
		FVector DoorLoc(InnDoor.X, InnDoor.Y, WallPivotZ);
		if (bHasDoorSocket)
		{
			DoorLoc = FrontLoc + FacePlaza.RotateVector(SocketLocal);
			DoorLoc.Z = WallPivotZ;
		}
		const FBoxSphereBounds DoorB = EnvDoor->GetBounds();
		const float DoorBottomLocal = static_cast<float>(DoorB.Origin.Z - DoorB.BoxExtent.Z);
		DoorLoc.Z = GetValleyFloorZCm() - DoorBottomLocal;
		AddEnvMesh(TEXT("InnEnvDoor"), EnvDoor, DoorLoc, FVector(1.f), FacePlaza, false);
	}

	const FVector BackLoc = FrontLoc - Forward * ModuleWidth;
	AddEnvMesh(TEXT("InnEnvWall_Back"), SideWallMesh, BackLoc, FVector(1.f), FacePlaza, false);

	const FVector CabinCenter = FrontLoc - Forward * HalfModule;
	AddEnvMesh(TEXT("InnEnvWall_L"), SideWallMesh, CabinCenter - Right * HalfModule, FVector(1.f), FaceSide, false);
	AddEnvMesh(TEXT("InnEnvWall_R"), SideWallMesh, CabinCenter + Right * HalfModule, FVector(1.f), FaceSide, false);

	if (EnvRoof)
	{
		const FBoxSphereBounds RoofB = EnvRoof->GetBounds();
		const float RoofBottomLocal = static_cast<float>(RoofB.Origin.Z - RoofB.BoxExtent.Z);
		const float RoofPivotZ = WallPivotZ + FrontTopLocal - RoofBottomLocal;
		AddEnvMesh(TEXT("InnEnvRoof"), EnvRoof, FVector(CabinCenter.X, CabinCenter.Y, RoofPivotZ), FVector(1.f), FacePlaza, false);
	}

	if (EnvPorch)
	{
		const FBoxSphereBounds PorchB = EnvPorch->GetBounds();
		const float PorchHalfDepth = FMath::Min(static_cast<float>(PorchB.BoxExtent.X), static_cast<float>(PorchB.BoxExtent.Y));
		const float PorchBottomLocal = static_cast<float>(PorchB.Origin.Z - PorchB.BoxExtent.Z);
		const float PorchPivotZ = GetValleyFloorZCm() - PorchBottomLocal;
		const FVector PorchLoc = FrontLoc + Forward * PorchHalfDepth;
		AddEnvMesh(TEXT("InnEnvPorch"), EnvPorch, FVector(PorchLoc.X, PorchLoc.Y, PorchPivotZ), FVector(1.f), FacePlaza, true);
	}
	else
	{
		AddCube(
			TEXT("InnEnvPorchFallback"),
			FrontLoc + Forward * HalfModule,
			FVector(ModuleWidth / 100.f, ModuleWidth / 100.f, 0.25f),
			FacePlaza,
			FLinearColor(0.32f, 0.24f, 0.18f),
			true);
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Mistspire MistInn: module=%.0fcm wallH=%.0fcm front=%s doorSocket=%s"),
		ModuleWidth,
		FrontTopLocal - FrontBottomLocal,
		*FrontWallMesh->GetName(),
		bHasDoorSocket ? TEXT("yes") : TEXT("none"));
}

void AMistspireDemoClimbScaffold::DressStationEnv(
	int32 StationIndex,
	const FVector& Station,
	const FVector& RadialOut,
	const FVector& Tangent,
	const FRotator& Yaw)
{
	using namespace MistspireDemoSpire;
	const FString Prefix = FString::Printf(TEXT("S%d_Env"), StationIndex);
	const int32 RockIdx = EnvRocks.Num() > 0 ? StationIndex % EnvRocks.Num() : -1;
	const float Ring = StationDressRingCm;

	auto PlaceDressRock = [&](const TCHAR* Suffix, int32 MeshIdx, const FVector& Offset, float Scale)
	{
		if (MeshIdx < 0 || MeshIdx >= EnvRocks.Num())
		{
			return;
		}
		AddEnvMesh(*(Prefix + Suffix), EnvRocks[MeshIdx], Station + Offset, FVector(Scale), Yaw, false);
	};

	switch (StationIndex)
	{
	case 0:
		PlaceDressRock(TEXT("RockL"), RockIdx, RadialOut * Ring + Tangent * 280.f, 1.3f);
		if (EnvRocks.Num() > 1)
		{
			PlaceDressRock(TEXT("RockR"), (RockIdx + 1) % EnvRocks.Num(), RadialOut * Ring - Tangent * 280.f, 1.15f);
		}
		break;
	case 1:
		PlaceDressRock(TEXT("MesaRock"), RockIdx, RadialOut * (Ring + 40.f), 1.8f);
		break;
	case 2:
		PlaceDressRock(TEXT("Rock"), RockIdx, RadialOut * Ring + Tangent * 120.f, 1.0f);
		break;
	case 3:
		PlaceDressRock(TEXT("EmberRock"), RockIdx, RadialOut * Ring - Tangent * 200.f, 1.35f);
		break;
	case 4:
		PlaceDressRock(TEXT("CrystalRock"), RockIdx, RadialOut * Ring + Tangent * 160.f, 1.25f);
		break;
	case 5:
		PlaceDressRock(TEXT("VoidRock"), RockIdx, RadialOut * Ring, 1.4f);
		break;
	case 6:
		// Rocks only above the valley: Iceland terrain patches floated here read as slices.
		PlaceDressRock(TEXT("IceRock"), RockIdx, RadialOut * Ring, 1.5f);
		if (EnvRocks.Num() > 1)
		{
			PlaceDressRock(TEXT("IceRockB"), (RockIdx + 2) % EnvRocks.Num(), RadialOut * (Ring - 60.f) + Tangent * 240.f, 1.1f);
		}
		break;
	case 7:
		PlaceDressRock(TEXT("AetherRock"), RockIdx, RadialOut * Ring + Tangent * 100.f, 1.2f);
		break;
	case 8:
		PlaceDressRock(TEXT("SanctumRock"), RockIdx, RadialOut * Ring, 1.3f);
		break;
	case 9:
		PlaceDressRock(TEXT("PeakRock"), RockIdx, RadialOut * Ring, 1.5f);
		if (EnvRocks.Num() > 2)
		{
			PlaceDressRock(TEXT("PeakRockB"), (RockIdx + 3) % EnvRocks.Num(), RadialOut * (Ring - 50.f) - Tangent * 220.f, 1.2f);
		}
		break;
	default:
		break;
	}
}

void AMistspireDemoClimbScaffold::BuildValley()
{
	using namespace MistspireDemoSpire;
	const FLinearColor MistTint = GetBiomeTint(0);
	const FRotator Identity = FRotator::ZeroRotator;

	// Wider walkable deck — SummitMass is the visual mountain under/around this pad.
	AddCube(TEXT("ShelfPad"), Valley(0.f, 0.f, -20.f), FVector(55.f, 55.f, 0.45f), Identity, FLinearColor(0.22f, 0.24f, 0.26f));
	const FVector Gate = GetValleyGateLocation();
	AddCube(TEXT("ArchPillarL"), Gate + FVector(0.f, -400.f, 300.f), FVector(1.2f, 1.2f, 6.f), Identity, MistTint);
	AddCube(TEXT("ArchPillarR"), Gate + FVector(0.f, 400.f, 300.f), FVector(1.2f, 1.2f, 6.f), Identity, MistTint);
	AddCube(TEXT("ArchLintel"), Gate + FVector(0.f, 0.f, 620.f), FVector(1.4f, 9.f, 1.f), Identity, MistTint);
	AddCube(TEXT("BrazierPlinth"), Gate + FVector(150.f, 0.f, 40.f), FVector(1.f, 1.f, 0.8f), Identity, FLinearColor(0.2f, 0.35f, 0.7f));

	// Grapple handoff landmark at end of walkable approach.
	AddCube(TEXT("HandoffBeacon"), Valley(2200.f, 0.f, ApproachEndZCm + 80.f), FVector(1.2f, 1.2f, 4.f), Identity, FLinearColor(0.95f, 0.75f, 0.2f), false);

	const FVector InnDoor = GetMistInnDoorLocation();
	AddCube(TEXT("InnPorchMarker"), FVector(InnDoor.X, InnDoor.Y + 200.f, GetValleyFloorZCm()), FVector(3.5f, 3.5f, 0.2f), FRotator(0.f, 90.f, 0.f), FLinearColor(0.35f, 0.28f, 0.22f));

	UPointLightComponent* Brazier = NewObject<UPointLightComponent>(
		this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), TEXT("ValleyBrazierLight")));
	Brazier->SetupAttachment(Root);
	Brazier->SetWorldLocation(Gate + FVector(150.f, 0.f, 120.f));
	Brazier->SetLightColor(GetBiomeLightColor(0));
	Brazier->SetIntensity(12000.f);
	Brazier->SetAttenuationRadius(1600.f);
	Brazier->RegisterComponent();

	UPointLightComponent* InnGlow = NewObject<UPointLightComponent>(
		this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), TEXT("InnPorchLight")));
	InnGlow->SetupAttachment(Root);
	InnGlow->SetWorldLocation(InnDoor + FVector(0.f, 0.f, 280.f));
	InnGlow->SetLightColor(FLinearColor(1.f, 0.72f, 0.4f));
	InnGlow->SetIntensity(7000.f);
	InnGlow->SetAttenuationRadius(1000.f);
	InnGlow->RegisterComponent();

	if (bSpawnEnvDress)
	{
		DressValleyEnv();
		DressMistInnPorchEnv();
	}
}

void AMistspireDemoClimbScaffold::BuildMistInnPocket()
{
	using namespace MistspireDemoSpire;
	const FVector Origin = GetMistInnInteriorSpawn();
	const FRotator Identity = FRotator::ZeroRotator;
	const FLinearColor WarmWood(0.42f, 0.30f, 0.18f);
	const FLinearColor WarmFloor(0.28f, 0.22f, 0.16f);
	const FLinearColor Hearth(0.55f, 0.25f, 0.12f);

	// Pocket room (~8×6×4 m). Exit is a door wall on +X (not an open void); InteriorExit sits
	// in that doorway so walking through returns to the valley porch.
	AddCube(TEXT("InnFloor"), Origin + FVector(0.f, 0.f, -20.f), FVector(8.f, 6.f, 0.4f), Identity, WarmFloor);
	AddCube(TEXT("InnCeiling"), Origin + FVector(0.f, 0.f, 400.f), FVector(8.f, 6.f, 0.3f), Identity, WarmWood);
	AddCube(TEXT("InnWallBack"), Origin + FVector(-400.f, 0.f, 180.f), FVector(0.3f, 6.f, 4.f), Identity, WarmWood);
	AddCube(TEXT("InnWallL"), Origin + FVector(-50.f, -300.f, 180.f), FVector(7.f, 0.3f, 4.f), Identity, WarmWood);
	AddCube(TEXT("InnWallR"), Origin + FVector(-50.f, 300.f, 180.f), FVector(7.f, 0.3f, 4.f), Identity, WarmWood);

	UStaticMesh* ExitWallMesh = EnvWallDoor ? EnvWallDoor.Get() : EnvWall.Get();
	if (ExitWallMesh)
	{
		const FBoxSphereBounds WB = ExitWallMesh->GetBounds();
		const float BottomLocal = static_cast<float>(WB.Origin.Z - WB.BoxExtent.Z);
		const float WallPivotZ = Origin.Z - BottomLocal;
		// Face back into the room (-X) so the opening reads as an interior exit door.
		const FRotator FaceInside(0.f, 180.f, 0.f);
		const FVector ExitWallLoc(Origin.X + 400.f, Origin.Y, WallPivotZ);
		AddEnvMesh(TEXT("InnExitWall"), ExitWallMesh, ExitWallLoc, FVector(1.f), FaceInside, false);

		if (EnvDoor)
		{
			const FBoxSphereBounds DoorB = EnvDoor->GetBounds();
			const float DoorBottomLocal = static_cast<float>(DoorB.Origin.Z - DoorB.BoxExtent.Z);
			const FVector DoorLoc(Origin.X + 400.f, Origin.Y, Origin.Z - DoorBottomLocal);
			AddEnvMesh(TEXT("InnExitDoor"), EnvDoor, DoorLoc, FVector(1.f), FaceInside, false);
		}
	}
	else
	{
		// Greybox door wall: solid flanks + lintel, open center for the exit volume.
		AddCube(TEXT("InnExitFlankL"), Origin + FVector(400.f, -180.f, 180.f), FVector(0.3f, 2.4f, 4.f), Identity, WarmWood);
		AddCube(TEXT("InnExitFlankR"), Origin + FVector(400.f, 180.f, 180.f), FVector(0.3f, 2.4f, 4.f), Identity, WarmWood);
		AddCube(TEXT("InnExitLintel"), Origin + FVector(400.f, 0.f, 360.f), FVector(0.3f, 1.2f, 0.8f), Identity, WarmWood);
	}

	// Keep hearth/table away from the +X exit corridor.
	AddCube(TEXT("InnHearth"), Origin + FVector(-280.f, 0.f, 80.f), FVector(1.2f, 2.f, 1.6f), Identity, Hearth);
	AddCube(TEXT("InnTable"), Origin + FVector(-40.f, 140.f, 50.f), FVector(1.5f, 1.f, 0.8f), Identity, WarmWood);

	UPointLightComponent* HearthLight = NewObject<UPointLightComponent>(
		this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), TEXT("InnHearthLight")));
	HearthLight->SetupAttachment(Root);
	HearthLight->SetWorldLocation(Origin + FVector(-250.f, 0.f, 160.f));
	HearthLight->SetLightColor(FLinearColor(1.f, 0.55f, 0.25f));
	HearthLight->SetIntensity(9000.f);
	HearthLight->SetAttenuationRadius(1200.f);
	HearthLight->RegisterComponent();
}

void AMistspireDemoClimbScaffold::SpawnValleyImmersionProps()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = this;

	// Button stands on the plaza edge of the porch, clear of the enter volume below. At
	// Door + 200Y it sat *inside* the trigger box, so walking up to press it teleported the
	// player into the inn instead of cycling the weather.
	const FVector InnDoor = MistspireDemoSpire::GetMistInnDoorLocation();
	if (AMistspirePhysicalButton* WeatherBtn = World->SpawnActor<AMistspirePhysicalButton>(
		InnDoor + FVector(-140.f, 340.f, 40.f), FRotator(0.f, 90.f, 0.f), Params))
	{
		WeatherBtn->BuiltInAction = EMistspireButtonAction::CycleWeather;
#if WITH_EDITOR
		WeatherBtn->SetActorLabel(TEXT("DemoWeatherButton"));
#endif
		SpawnedPropActors.Add(WeatherBtn);
	}

	// Refresh / spawn Mist Inn enter volume. Walking into it teleports to the pocket room at
	// GetMistInnInteriorSpawn(); the exterior cabin is only a shell.
	if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		Atlas->SeedProductionWorld();
		Atlas->SpawnAuthoredWorldMarkers();
	}
	const FName MistInnId(TEXT("building_valley_inn"));
	const FVector Trigger = MistspireDemoSpire::GetMistInnDoorTriggerLocation();
	bool bFoundInnDoor = false;
	for (TActorIterator<AMistspireBuildingEntrance> It(World); It; ++It)
	{
		if (It->BuildingId != MistInnId)
		{
			continue;
		}
		bFoundInnDoor = true;
		It->SetActorLocationAndRotation(Trigger, FRotator(0.f, 90.f, 0.f));
		if (UBoxComponent* Box = It->FindComponentByClass<UBoxComponent>())
		{
			// Generous trigger on the plaza side of the opening.
			Box->SetBoxExtent(FVector(100.f, 180.f, 220.f));
		}
		// Hide the greybox door frame — Wall_Door_4m is the visible entrance.
		TArray<UStaticMeshComponent*> Meshes;
		It->GetComponents<UStaticMeshComponent>(Meshes);
		for (UStaticMeshComponent* MeshComp : Meshes)
		{
			if (MeshComp)
			{
				MeshComp->SetVisibility(false);
			}
		}
		break;
	}
	if (!bFoundInnDoor)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Mistspire MistInn: no building_valley_inn entrance — walk-in teleport unavailable. Enable -demoworld / mistspire.DemoMode 1."));
	}

	// Keep the pocket exit volume seated in the exit door opening (atlas may have spawned it at the old +200 offset).
	const FVector ExitLoc = MistspireDemoSpire::GetMistInnInteriorExitLocation();
	bool bFoundExit = false;
	for (TActorIterator<AMistspireInteriorExit> It(World); It; ++It)
	{
		const FVector Loc = It->GetActorLocation();
		if (FVector::DistSquared(Loc, MistspireDemoSpire::GetMistInnInteriorSpawn()) < FMath::Square(2000.f)
			|| FVector::DistSquared(Loc, ExitLoc) < FMath::Square(2000.f))
		{
			It->SetActorLocation(ExitLoc);
			bFoundExit = true;
			break;
		}
	}
	if (!bFoundExit)
	{
		if (AMistspireInteriorExit* Exit = World->SpawnActor<AMistspireInteriorExit>(ExitLoc, FRotator::ZeroRotator, Params))
		{
			SpawnedPropActors.Add(Exit);
		}
	}
	const FVector Interior = MistspireDemoSpire::GetMistInnInteriorSpawn();
	if (AMistspireRestShelter* Shelter = World->SpawnActor<AMistspireRestShelter>(
		Interior + FVector(120.f, -80.f, 40.f), FRotator::ZeroRotator, Params))
	{
#if WITH_EDITOR
		Shelter->SetActorLabel(TEXT("DemoInnShelter"));
#endif
		SpawnedPropActors.Add(Shelter);
	}

	if (AMistspireLoreShard* Shard = World->SpawnActor<AMistspireLoreShard>(
		Interior + FVector(-100.f, 140.f, 60.f), FRotator::ZeroRotator, Params))
	{
		Shard->LoreTitle = NSLOCTEXT("Mistspire", "InnLoreTitle", "Mist Inn");
		Shard->LoreBody = NSLOCTEXT("Mistspire", "InnLoreBody", "Warmth below the Gate. Climb when the mist thins.");
#if WITH_EDITOR
		Shard->SetActorLabel(TEXT("DemoInnLore"));
#endif
		SpawnedPropActors.Add(Shard);
	}
}

void AMistspireDemoClimbScaffold::BuildCentralMast()
{
	using namespace MistspireDemoSpire;
	const FLinearColor MastTint(0.35f, 0.38f, 0.45f);
	const float FloorZ = GetValleyFloorZCm();
	AddCylinder(
		TEXT("CentralMast"),
		FVector(GetValleyOrigin().X, GetValleyOrigin().Y, FloorZ + StationAltitudeCm[9] * 0.5f),
		FVector(2.5f, 2.5f, StationAltitudeCm[9] / 100.f),
		FRotator::ZeroRotator,
		MastTint,
		false);

	for (int32 i = 0; i < StationCount; ++i)
	{
		UPointLightComponent* Beacon = NewObject<UPointLightComponent>(
			this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), *FString::Printf(TEXT("MastBeacon_%d"), i)));
		Beacon->SetupAttachment(Root);
		Beacon->SetWorldLocation(FVector(GetValleyOrigin().X, GetValleyOrigin().Y, FloorZ + StationAltitudeCm[i]));
		Beacon->SetLightColor(GetBiomeLightColor(i));
		Beacon->SetIntensity(25000.f);
		Beacon->SetAttenuationRadius(6000.f);
		Beacon->RegisterComponent();
	}
}

void AMistspireDemoClimbScaffold::BuildApproachHelix()
{
	using namespace MistspireDemoSpire;
	const FLinearColor Tint = GetBiomeTint(0);
	const float StartAngleDeg = -ApproachSweepDeg;
	const float EndAngleDeg = 0.f;
	const float StartRadius = 2200.f;
	const float EndRadius = HelixRadiusCm;
	const float StepZ = ApproachStepZCm;
	const float FloorZ = GetValleyFloorZCm();
	const FVector Origin = GetValleyOrigin();

	int32 Step = 0;
	for (float Z = 0.f; Z <= ApproachEndZCm + 1.f; Z += StepZ)
	{
		const float T = FMath::Clamp(
			(ApproachEndZCm > 0.f) ? (Z / ApproachEndZCm) : 1.f,
			0.f, 1.f);
		const float AngleDeg = FMath::Lerp(StartAngleDeg, EndAngleDeg, T);
		const float AngleRad = FMath::DegreesToRadians(AngleDeg);
		const float Radius = FMath::Lerp(StartRadius, EndRadius, T);
		const FRotator Yaw(0.f, AngleDeg, 0.f);
		const FVector Loc(
			Origin.X + Radius * FMath::Cos(AngleRad),
			Origin.Y + Radius * FMath::Sin(AngleRad),
			FloorZ + Z);

		AddCube(*FString::Printf(TEXT("ApproachPad_%d"), Step), Loc, FVector(1.5f, 1.5f, 0.4f), Yaw, Tint);

		if (Step % 10 == 0)
		{
			AddCube(
				*FString::Printf(TEXT("ApproachPeg_%d"), Step),
				Loc + FVector(0.f, 0.f, 80.f),
				FVector(0.7f, 0.7f, 0.7f),
				Yaw,
				FLinearColor(0.95f, 0.85f, 0.2f),
				false);
		}
		++Step;
	}
}

void AMistspireDemoClimbScaffold::BuildGrappleShaftToMist()
{
	using namespace MistspireDemoSpire;
	const FLinearColor Tint = GetBiomeTint(0);
	const FVector Station0 = GetStationLocation(0);
	const FRotator Yaw(0.f, 0.f, 0.f);
	const float FloorZ = GetValleyFloorZCm();
	const float MistVignetteStart = StationAltitudeCm[0] - VignetteHeightCm;
	constexpr float GapZ = 4500.f;

	int32 Peg = 0;
	for (float Z = ApproachEndZCm + GapZ; Z < MistVignetteStart - 100.f; Z += GapZ)
	{
		const float Side = (Peg % 2 == 0) ? 400.f : -400.f;
		const FVector PadLoc(Station0.X, Station0.Y + Side, FloorZ + Z);
		AddCube(*FString::Printf(TEXT("ShaftPad_%d"), Peg), PadLoc, FVector(3.5f, 3.5f, 0.35f), Yaw, Tint);
		AddCube(
			*FString::Printf(TEXT("ShaftPeg_%d"), Peg),
			PadLoc + FVector(0.f, 0.f, 120.f),
			FVector(1.f, 1.f, 1.f),
			Yaw,
			FLinearColor(1.f, 0.9f, 0.15f),
			false);
		++Peg;
	}
}

void AMistspireDemoClimbScaffold::BuildStation(int32 StationIndex)
{
	const FVector Station = MistspireDemoSpire::GetStationLocation(StationIndex);
	const FLinearColor Tint = MistspireDemoSpire::GetBiomeTint(StationIndex);
	const float YawDeg = MistspireDemoSpire::GetStationYawDeg(StationIndex);
	const float AngleRad = FMath::DegreesToRadians(YawDeg);
	const FRotator Yaw(0.f, YawDeg, 0.f);
	const FVector RadialOut(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f);
	const FVector Tangent(-FMath::Sin(AngleRad), FMath::Cos(AngleRad), 0.f);
	const FString Prefix = FString::Printf(TEXT("S%d_"), StationIndex);

	AddCube(*(Prefix + TEXT("Pad")), Station - FVector(0.f, 0.f, 20.f), FVector(6.f, 6.f, 0.4f), Yaw, Tint);

	// Comfort rails on all four pad edges (~120 cm tall).
	AddCube(*(Prefix + TEXT("RailA")), Station + Tangent * 280.f + FVector(0.f, 0.f, 60.f), FVector(5.5f, 0.25f, 1.2f), Yaw, Tint * 0.7f);
	AddCube(*(Prefix + TEXT("RailB")), Station - Tangent * 280.f + FVector(0.f, 0.f, 60.f), FVector(5.5f, 0.25f, 1.2f), Yaw, Tint * 0.7f);
	AddCube(*(Prefix + TEXT("RailInner")), Station - RadialOut * 280.f + FVector(0.f, 0.f, 60.f), FVector(0.25f, 5.5f, 1.2f), Yaw, Tint * 0.7f);

	// Walkable stairs (40 cm): radial advance only so the run terminates onto the deck.
	constexpr int32 StairSteps = 12;
	const float StairZ = MistspireDemoSpire::ApproachStepZCm;
	const float BaseZ = Station.Z - MistspireDemoSpire::VignetteHeightCm;
	for (int32 Step = 0; Step < StairSteps; ++Step)
	{
		const float Z = BaseZ + static_cast<float>(Step) * StairZ;
		// 1430 → 330 over 11 steps (100 cm/step): last tread overlaps the deck (≥ capsule radius).
		const float OutBias = 1430.f - static_cast<float>(Step) * 100.f;
		const FVector PadLoc = FVector(Station.X, Station.Y, Z) + RadialOut * OutBias;
		AddCube(*FString::Printf(TEXT("%sVPad_%d"), *Prefix, Step), PadLoc, FVector(1.8f, 1.8f, 0.4f), Yaw, Tint);
		if (Step % 3 == 0)
		{
			AddCube(
				*FString::Printf(TEXT("%sPeg_%d"), *Prefix, Step),
				PadLoc + FVector(0.f, 0.f, 90.f),
				FVector(0.7f, 0.7f, 0.7f),
				Yaw,
				FLinearColor(1.f, 0.9f, 0.2f),
				false);
		}
	}

	switch (StationIndex)
	{
	case 0:
		AddCube(*(Prefix + TEXT("SilL")), Station + RadialOut * 200.f + Tangent * 180.f + FVector(0.f, 0.f, 200.f), FVector(0.8f, 0.8f, 4.f), Yaw, Tint);
		AddCube(*(Prefix + TEXT("SilR")), Station + RadialOut * 200.f - Tangent * 180.f + FVector(0.f, 0.f, 200.f), FVector(0.8f, 0.8f, 4.f), Yaw, Tint);
		AddCube(*(Prefix + TEXT("SilTop")), Station + RadialOut * 200.f + FVector(0.f, 0.f, 420.f), FVector(0.9f, 4.f, 0.7f), Yaw, Tint);
		break;
	case 1:
		AddCube(*(Prefix + TEXT("Mesa")), Station + RadialOut * 250.f + FVector(0.f, 0.f, 150.f), FVector(4.f, 4.f, 3.f), Yaw, Tint);
		break;
	case 2:
		AddCylinder(*(Prefix + TEXT("TrunkA")), Station + Tangent * 200.f + FVector(0.f, 0.f, 200.f), FVector(1.2f, 1.2f, 4.f), Yaw, Tint);
		AddCylinder(*(Prefix + TEXT("TrunkB")), Station - Tangent * 180.f + FVector(0.f, 0.f, 160.f), FVector(1.f, 1.f, 3.2f), Yaw, Tint);
		break;
	case 3:
		AddCube(*(Prefix + TEXT("Obsidian")), Station + RadialOut * 220.f + FVector(0.f, 0.f, 120.f), FVector(3.f, 1.5f, 2.5f), Yaw, Tint);
		break;
	case 4:
		AddCube(*(Prefix + TEXT("ShardA")), Station + Tangent * 150.f + FVector(0.f, 0.f, 250.f), FVector(0.8f, 0.8f, 5.f), Yaw, Tint);
		AddCube(*(Prefix + TEXT("ShardB")), Station - Tangent * 120.f + FVector(0.f, 0.f, 200.f), FVector(0.6f, 0.6f, 4.f), Yaw, Tint);
		break;
	case 5:
		// 7 m pier jutting outward from the deck edge. At RadialOut*300 with an 8 m span it
		// reached back across the pad centre and the DemoTour landing started inside the slab.
		AddCube(*(Prefix + TEXT("Pier")), Station + RadialOut * 420.f + FVector(0.f, 0.f, 40.f), FVector(7.f, 1.2f, 0.35f), Yaw, Tint);
		break;
	case 6:
		AddCube(*(Prefix + TEXT("Obelisk")), Station + RadialOut * 180.f + FVector(0.f, 0.f, 400.f), FVector(1.2f, 1.2f, 8.f), Yaw, Tint);
		break;
	case 7:
		AddCube(*(Prefix + TEXT("Span")), Station + RadialOut * 200.f + FVector(0.f, 0.f, 80.f), FVector(1.f, 10.f, 0.35f), Yaw, Tint);
		break;
	case 8:
		AddCube(*(Prefix + TEXT("Plinth")), Station + RadialOut * 160.f + FVector(0.f, 0.f, 80.f), FVector(3.f, 3.f, 1.5f), Yaw, Tint);
		break;
	case 9:
		AddCylinder(*(Prefix + TEXT("Needle")), Station + FVector(0.f, 0.f, 600.f), FVector(0.9f, 0.9f, 12.f), Yaw, Tint);
		break;
	default:
		break;
	}

	if (bSpawnEnvDress)
	{
		DressStationEnv(StationIndex, Station, RadialOut, Tangent, Yaw);
	}

	UPointLightComponent* Light = NewObject<UPointLightComponent>(
		this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), *(Prefix + TEXT("Light"))));
	Light->SetupAttachment(Root);
	Light->SetWorldLocation(Station + FVector(0.f, 0.f, 250.f));
	Light->SetLightColor(MistspireDemoSpire::GetBiomeLightColor(StationIndex));
	Light->SetIntensity(StationIndex >= 3 ? 18000.f : 10000.f);
	Light->SetAttenuationRadius(2400.f);
	Light->RegisterComponent();

	if (bSpawnImmersionProps)
	{
		SpawnImmersionForStation(StationIndex, Station);
	}
}

void AMistspireDemoClimbScaffold::SpawnImmersionForStation(int32 StationIndex, const FVector& StationLoc)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = this;

	const float AngleRad = FMath::DegreesToRadians(MistspireDemoSpire::GetStationYawDeg(StationIndex));
	const FVector Tangent(-FMath::Sin(AngleRad), FMath::Cos(AngleRad), 0.f);
	const FVector RadialOut(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f);

	if (AMistspireSummitMarker* Marker = World->SpawnActor<AMistspireSummitMarker>(
		StationLoc + FVector(0.f, 0.f, 120.f), FRotator::ZeroRotator, Params))
	{
		Marker->SummitId = MistspireDemoSpire::GetSummitId(StationIndex);
		Marker->OfficialAltitudeCm = MistspireDemoSpire::StationAltitudeCm[StationIndex];
		Marker->ReachRadiusCm = 700.f;
#if WITH_EDITOR
		Marker->SetActorLabel(FString::Printf(TEXT("DemoSummit_%s"), MistspireDemoSpire::BiomeNames[StationIndex]));
#endif
		SpawnedPropActors.Add(Marker);
	}

	// Shelters from Ember upward so early stations keep survival tension on camera.
	if (StationIndex >= 3)
	{
		if (AMistspireRestShelter* Shelter = World->SpawnActor<AMistspireRestShelter>(
			StationLoc + Tangent * 220.f + FVector(0.f, 0.f, 40.f), FRotator::ZeroRotator, Params))
		{
#if WITH_EDITOR
			Shelter->SetActorLabel(FString::Printf(TEXT("DemoShelter_%s"), MistspireDemoSpire::BiomeNames[StationIndex]));
#endif
			SpawnedPropActors.Add(Shelter);
		}

		if (AMistspireOxygenCanister* O2 = World->SpawnActor<AMistspireOxygenCanister>(
			StationLoc - Tangent * 180.f + FVector(0.f, 0.f, 60.f), FRotator::ZeroRotator, Params))
		{
#if WITH_EDITOR
			O2->SetActorLabel(FString::Printf(TEXT("DemoO2_%s"), MistspireDemoSpire::BiomeNames[StationIndex]));
#endif
			SpawnedPropActors.Add(O2);
		}
	}

	if (StationIndex == 2 || StationIndex == 7 || StationIndex == 9)
	{
		if (AMistspireWindCrystal* Crystal = World->SpawnActor<AMistspireWindCrystal>(
			StationLoc + RadialOut * 150.f + FVector(0.f, 0.f, 70.f), FRotator::ZeroRotator, Params))
		{
#if WITH_EDITOR
			Crystal->SetActorLabel(FString::Printf(TEXT("DemoWind_%s"), MistspireDemoSpire::BiomeNames[StationIndex]));
#endif
			SpawnedPropActors.Add(Crystal);
		}
	}

	if (StationIndex == 0)
	{
		if (AMistspireLoreShard* Shard = World->SpawnActor<AMistspireLoreShard>(
			MistspireDemoSpire::Valley(400.f, 200.f, 80.f), FRotator::ZeroRotator, Params))
		{
			Shard->LoreTitle = NSLOCTEXT("Mistspire", "DemoLoreTitle", "Valley Gate");
			Shard->LoreBody = NSLOCTEXT("Mistspire", "DemoLoreBody", "Every step after the Gate is an ascent.");
#if WITH_EDITOR
			Shard->SetActorLabel(TEXT("DemoLore_ValleyGate"));
#endif
			SpawnedPropActors.Add(Shard);
		}
	}
}
