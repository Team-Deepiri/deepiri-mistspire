#include "MistspireDemoClimbScaffold.h"
#include "MistspireDemoSpireLayout.h"
#include "MistspireSummitMarker.h"
#include "MistspireRestShelter.h"
#include "MistspireOxygenCanister.h"
#include "MistspireWindCrystal.h"
#include "MistspireLoreShard.h"
#include "MistspirePhysicalButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
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
			Existing->Rebuild();
			return Existing;
		}
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	return World->SpawnActor<AMistspireDemoClimbScaffold>(
		AMistspireDemoClimbScaffold::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
}

void AMistspireDemoClimbScaffold::Rebuild()
{
	ClearBuiltActors();
	CachedTintMIDs.Reset();

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

	if (bSpawnDistantSilhouettes)
	{
		BuildDistantSilhouettes();
	}

	if (bSpawnImmersionProps)
	{
		SpawnValleyImmersionProps();
	}

	UE_LOG(LogTemp, Log, TEXT("Mistspire DemoClimbScaffold: rebuilt %d stations + valley/Mist Inn/approach/shaft."),
		MistspireDemoSpire::StationCount);
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

void AMistspireDemoClimbScaffold::BuildValley()
{
	const FLinearColor MistTint = MistspireDemoSpire::GetBiomeTint(0);
	const FRotator Identity = FRotator::ZeroRotator;

	AddCube(TEXT("ValleyFloor"), FVector(0.f, 0.f, -20.f), FVector(80.f, 80.f, 0.4f), Identity, MistTint);
	AddCube(TEXT("ArchPillarL"), FVector(-250.f, -400.f, 300.f), FVector(1.2f, 1.2f, 6.f), Identity, MistTint);
	AddCube(TEXT("ArchPillarR"), FVector(-250.f, 400.f, 300.f), FVector(1.2f, 1.2f, 6.f), Identity, MistTint);
	AddCube(TEXT("ArchLintel"), FVector(-250.f, 0.f, 620.f), FVector(1.4f, 9.f, 1.f), Identity, MistTint);
	AddCube(TEXT("BrazierPlinth"), FVector(-100.f, 0.f, 40.f), FVector(1.f, 1.f, 0.8f), Identity, FLinearColor(0.2f, 0.35f, 0.7f));

	// Mist Inn porch marker near the atlas door (readable from valley spawn).
	const FVector InnDoor = MistspireDemoSpire::GetMistInnDoorLocation();
	AddCube(TEXT("InnPorch"), InnDoor + FVector(0.f, 0.f, -10.f), FVector(4.f, 4.f, 0.2f), Identity, FLinearColor(0.35f, 0.28f, 0.22f));
	AddCube(TEXT("InnFrameL"), InnDoor + FVector(0.f, -90.f, 160.f), FVector(0.4f, 0.4f, 3.2f), Identity, FLinearColor(0.4f, 0.3f, 0.22f));
	AddCube(TEXT("InnFrameR"), InnDoor + FVector(0.f, 90.f, 160.f), FVector(0.4f, 0.4f, 3.2f), Identity, FLinearColor(0.4f, 0.3f, 0.22f));
	AddCube(TEXT("InnLintel"), InnDoor + FVector(0.f, 0.f, 330.f), FVector(0.5f, 2.2f, 0.4f), Identity, FLinearColor(0.45f, 0.32f, 0.2f));

	UPointLightComponent* Brazier = NewObject<UPointLightComponent>(
		this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), TEXT("ValleyBrazierLight")));
	Brazier->SetupAttachment(Root);
	Brazier->SetWorldLocation(FVector(-100.f, 0.f, 120.f));
	Brazier->SetLightColor(MistspireDemoSpire::GetBiomeLightColor(0));
	Brazier->SetIntensity(10000.f);
	Brazier->SetAttenuationRadius(1400.f);
	Brazier->RegisterComponent();

	UPointLightComponent* InnGlow = NewObject<UPointLightComponent>(
		this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), TEXT("InnPorchLight")));
	InnGlow->SetupAttachment(Root);
	InnGlow->SetWorldLocation(InnDoor + FVector(0.f, 0.f, 280.f));
	InnGlow->SetLightColor(FLinearColor(1.f, 0.72f, 0.4f));
	InnGlow->SetIntensity(6000.f);
	InnGlow->SetAttenuationRadius(900.f);
	InnGlow->RegisterComponent();
}

void AMistspireDemoClimbScaffold::BuildMistInnPocket()
{
	const FVector Origin = MistspireDemoSpire::GetMistInnInteriorSpawn();
	const FRotator Identity = FRotator::ZeroRotator;
	const FLinearColor WarmWood(0.42f, 0.30f, 0.18f);
	const FLinearColor WarmFloor(0.28f, 0.22f, 0.16f);
	const FLinearColor Hearth(0.55f, 0.25f, 0.12f);

	// Simple pocket room (~8×6×4 m) around atlas interior spawn.
	AddCube(TEXT("InnFloor"), Origin + FVector(0.f, 0.f, -20.f), FVector(8.f, 6.f, 0.4f), Identity, WarmFloor);
	AddCube(TEXT("InnCeiling"), Origin + FVector(0.f, 0.f, 400.f), FVector(8.f, 6.f, 0.3f), Identity, WarmWood);
	AddCube(TEXT("InnWallBack"), Origin + FVector(-400.f, 0.f, 180.f), FVector(0.3f, 6.f, 4.f), Identity, WarmWood);
	AddCube(TEXT("InnWallL"), Origin + FVector(0.f, -300.f, 180.f), FVector(8.f, 0.3f, 4.f), Identity, WarmWood);
	AddCube(TEXT("InnWallR"), Origin + FVector(0.f, 300.f, 180.f), FVector(8.f, 0.3f, 4.f), Identity, WarmWood);
	AddCube(TEXT("InnHearth"), Origin + FVector(-280.f, 0.f, 80.f), FVector(1.2f, 2.f, 1.6f), Identity, Hearth);
	AddCube(TEXT("InnTable"), Origin + FVector(80.f, 120.f, 50.f), FVector(1.5f, 1.f, 0.8f), Identity, WarmWood);

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

	const FVector InnDoor = MistspireDemoSpire::GetMistInnDoorLocation();
	if (AMistspirePhysicalButton* WeatherBtn = World->SpawnActor<AMistspirePhysicalButton>(
		InnDoor + FVector(-120.f, 160.f, 40.f), FRotator::ZeroRotator, Params))
	{
		WeatherBtn->BuiltInAction = EMistspireButtonAction::CycleWeather;
#if WITH_EDITOR
		WeatherBtn->SetActorLabel(TEXT("DemoWeatherButton"));
#endif
		SpawnedPropActors.Add(WeatherBtn);
	}

	// Door + interior exit come from UMistspireWorldAtlasSubsystem::SpawnAuthoredWorldMarkers
	// (building_valley_inn now points at GetMistInnDoorLocation / GetMistInnInteriorSpawn).

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
	// Lit central column so the vertical journey reads in a single camera beat.
	const FLinearColor MastTint(0.35f, 0.38f, 0.45f);
	AddCylinder(
		TEXT("CentralMast"),
		FVector(0.f, 0.f, MistspireDemoSpire::StationAltitudeCm[9] * 0.5f),
		FVector(2.5f, 2.5f, MistspireDemoSpire::StationAltitudeCm[9] / 100.f),
		FRotator::ZeroRotator,
		MastTint,
		false);

	for (int32 i = 0; i < MistspireDemoSpire::StationCount; ++i)
	{
		UPointLightComponent* Beacon = NewObject<UPointLightComponent>(
			this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), *FString::Printf(TEXT("MastBeacon_%d"), i)));
		Beacon->SetupAttachment(Root);
		Beacon->SetWorldLocation(FVector(0.f, 0.f, MistspireDemoSpire::StationAltitudeCm[i]));
		Beacon->SetLightColor(MistspireDemoSpire::GetBiomeLightColor(i));
		Beacon->SetIntensity(25000.f);
		Beacon->SetAttenuationRadius(6000.f);
		Beacon->RegisterComponent();
	}
}

void AMistspireDemoClimbScaffold::BuildApproachHelix()
{
	// Walkable stairs: rise ≤45 cm; StartRadius 2200 + 450° sweep → ~115–131 cm tread advance.
	const FLinearColor Tint = MistspireDemoSpire::GetBiomeTint(0);
	const float StartAngleDeg = -MistspireDemoSpire::ApproachSweepDeg;
	const float EndAngleDeg = 0.f;
	const float StartRadius = 2200.f;
	const float EndRadius = MistspireDemoSpire::HelixRadiusCm;
	const float StepZ = MistspireDemoSpire::ApproachStepZCm;

	int32 Step = 0;
	for (float Z = 0.f; Z <= MistspireDemoSpire::ApproachEndZCm + 1.f; Z += StepZ)
	{
		const float T = FMath::Clamp(
			(MistspireDemoSpire::ApproachEndZCm > 0.f) ? (Z / MistspireDemoSpire::ApproachEndZCm) : 1.f,
			0.f, 1.f);
		const float AngleDeg = FMath::Lerp(StartAngleDeg, EndAngleDeg, T);
		const float AngleRad = FMath::DegreesToRadians(AngleDeg);
		const float Radius = FMath::Lerp(StartRadius, EndRadius, T);
		const FRotator Yaw(0.f, AngleDeg, 0.f);
		const FVector Loc(Radius * FMath::Cos(AngleRad), Radius * FMath::Sin(AngleRad), Z);

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
	// Floating grapple pads staggered off-axis so tops/sides are landable (not undersides).
	const FLinearColor Tint = MistspireDemoSpire::GetBiomeTint(0);
	const FVector Station0 = MistspireDemoSpire::GetStationLocation(0);
	const FRotator Yaw(0.f, 0.f, 0.f);
	const float MistVignetteStart = MistspireDemoSpire::StationAltitudeCm[0] - MistspireDemoSpire::VignetteHeightCm;
	constexpr float GapZ = 4500.f;

	int32 Peg = 0;
	for (float Z = MistspireDemoSpire::ApproachEndZCm + GapZ; Z < MistVignetteStart - 100.f; Z += GapZ)
	{
		const float Side = (Peg % 2 == 0) ? 400.f : -400.f;
		const FVector PadLoc(Station0.X, Station0.Y + Side, Z);
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
		AddCube(*(Prefix + TEXT("Pier")), Station + RadialOut * 300.f + FVector(0.f, 0.f, 40.f), FVector(8.f, 1.2f, 0.35f), Yaw, Tint);
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
			FVector(400.f, 200.f, 80.f), FRotator::ZeroRotator, Params))
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

void AMistspireDemoClimbScaffold::BuildDistantSilhouettes()
{
	for (int32 i = 0; i < MistspireDemoSpire::StationCount; ++i)
	{
		const float AngleRad = FMath::DegreesToRadians(static_cast<float>(i) * MistspireDemoSpire::AngleStepDeg + 18.f);
		const float Radius = 45000.f; // 450 m — reads as distant parallax
		const FVector Loc(
			Radius * FMath::Cos(AngleRad),
			Radius * FMath::Sin(AngleRad),
			MistspireDemoSpire::StationAltitudeCm[i] * 0.35f);
		const FLinearColor Tint = MistspireDemoSpire::GetBiomeTint(i) * 0.75f;
		AddCube(
			*FString::Printf(TEXT("Silhouette_%d"), i),
			Loc,
			FVector(40.f, 40.f, 100.f + static_cast<float>(i) * 16.f),
			FRotator(0.f, MistspireDemoSpire::GetStationYawDeg(i) + 18.f, 0.f),
			Tint,
			false);
		AddCube(
			*FString::Printf(TEXT("SilhouetteCap_%d"), i),
			Loc + FVector(0.f, 0.f, 500.f + static_cast<float>(i) * 80.f),
			FVector(18.f, 18.f, 25.f),
			FRotator(0.f, MistspireDemoSpire::GetStationYawDeg(i) + 18.f, 0.f),
			Tint * 1.15f,
			false);

		UPointLightComponent* Glow = NewObject<UPointLightComponent>(
			this, MakeUniqueObjectName(this, UPointLightComponent::StaticClass(), *FString::Printf(TEXT("SilhouetteGlow_%d"), i)));
		Glow->SetupAttachment(Root);
		Glow->SetWorldLocation(Loc + FVector(0.f, 0.f, 200.f));
		Glow->SetLightColor(MistspireDemoSpire::GetBiomeLightColor(i));
		Glow->SetIntensity(40000.f);
		Glow->SetAttenuationRadius(8000.f);
		Glow->RegisterComponent();
	}
}
