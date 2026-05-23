#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireBuildingEntrance.h"
#include "MistspireInteriorExit.h"
#include "MistspirePOIMarker.h"
#include "Engine/World.h"

FText UMistspireWorldAtlasSubsystem::GetDistrictDisplayName(EMistspireWorldDistrict District)
{
	switch (District)
	{
		case EMistspireWorldDistrict::ValleyHaven: return NSLOCTEXT("Mistspire", "DistValley", "Valley Haven");
		case EMistspireWorldDistrict::Mistmarket: return NSLOCTEXT("Mistspire", "DistMist", "Mistmarket");
		case EMistspireWorldDistrict::IronMesa: return NSLOCTEXT("Mistspire", "DistMesa", "Iron Mesa");
		case EMistspireWorldDistrict::CloudPromenade: return NSLOCTEXT("Mistspire", "DistCloud", "Cloud Promenade");
		case EMistspireWorldDistrict::SpireUnderworks: return NSLOCTEXT("Mistspire", "DistUnder", "Spire Underworks");
		case EMistspireWorldDistrict::RiftQuarter: return NSLOCTEXT("Mistspire", "DistRift", "Rift Quarter");
		case EMistspireWorldDistrict::EmberSlums: return NSLOCTEXT("Mistspire", "DistEmber", "Ember Slums");
		case EMistspireWorldDistrict::CathedralApproach: return NSLOCTEXT("Mistspire", "DistCathedral", "Cathedral Approach");
		case EMistspireWorldDistrict::ObservatoryRing: return NSLOCTEXT("Mistspire", "DistObservatory", "Observatory Ring");
		case EMistspireWorldDistrict::ZenithDock: return NSLOCTEXT("Mistspire", "DistZenith", "Zenith Dock");
		case EMistspireWorldDistrict::FrostArchive: return NSLOCTEXT("Mistspire", "DistFrost", "Frost Archive");
		case EMistspireWorldDistrict::StormBreak: return NSLOCTEXT("Mistspire", "DistStorm", "Storm Break");
		default: return NSLOCTEXT("Mistspire", "DistUnknown", "The Mist");
	}
}

void UMistspireWorldAtlasSubsystem::RegisterDistrict(const FMistspireDistrictEntry& Entry)
{
	for (FMistspireDistrictEntry& D : Districts)
	{
		if (D.DistrictId == Entry.DistrictId)
		{
			D = Entry;
			return;
		}
	}
	Districts.Add(Entry);
}

void UMistspireWorldAtlasSubsystem::RegisterBuilding(const FMistspireBuildingEntry& Entry)
{
	for (FMistspireBuildingEntry& B : Buildings)
	{
		if (B.BuildingId == Entry.BuildingId)
		{
			B = Entry;
			return;
		}
	}
	Buildings.Add(Entry);
}

void UMistspireWorldAtlasSubsystem::RegisterPOI(const FMistspirePOIEntry& Entry)
{
	for (FMistspirePOIEntry& P : POIs)
	{
		if (P.POIId == Entry.POIId)
		{
			P = Entry;
			return;
		}
	}
	POIs.Add(Entry);
}

EMistspireWorldDistrict UMistspireWorldAtlasSubsystem::GetDistrictAtLocation(const FVector& WorldLocation) const
{
	EMistspireWorldDistrict Best = EMistspireWorldDistrict::Unknown;
	float BestWeight = -1.f;

	for (const FMistspireDistrictEntry& D : Districts)
	{
		if (WorldLocation.Z < D.MinAltitudeCm || WorldLocation.Z > D.MaxAltitudeCm)
		{
			continue;
		}

		const FVector Local = WorldLocation - D.BoundsCenter;
		if (FMath::Abs(Local.X) > D.BoundsExtent.X || FMath::Abs(Local.Y) > D.BoundsExtent.Y)
		{
			continue;
		}

		const float Weight = 1.f / (1.f + Local.Size2D() / 100000.f);
		if (Weight > BestWeight)
		{
			BestWeight = Weight;
			Best = D.DistrictId;
		}
	}
	return Best;
}

bool UMistspireWorldAtlasSubsystem::FindBuilding(FName BuildingId, FMistspireBuildingEntry& OutEntry) const
{
	for (const FMistspireBuildingEntry& B : Buildings)
	{
		if (B.BuildingId == BuildingId)
		{
			OutEntry = B;
			return true;
		}
	}
	return false;
}

void UMistspireWorldAtlasSubsystem::UpdateDistrictFromPlayerLocation(const FVector& WorldLocation)
{
	const EMistspireWorldDistrict NewDistrict = GetDistrictAtLocation(WorldLocation);
	if (NewDistrict == CurrentDistrict)
	{
		return;
	}

	const EMistspireWorldDistrict Old = CurrentDistrict;
	CurrentDistrict = NewDistrict;
	OnDistrictEntered.Broadcast(Old, NewDistrict);
}

void UMistspireWorldAtlasSubsystem::SeedProductionWorld()
{
	Districts.Reset();
	Buildings.Reset();
	POIs.Reset();

	auto AddDistrict = [&](EMistspireWorldDistrict Id, const TCHAR* Name, FVector Center, FVector Extent, float MinZ, float MaxZ, int32 Slots, const TCHAR* Flavor)
	{
		FMistspireDistrictEntry D;
		D.DistrictId = Id;
		D.DistrictName = FName(Name);
		D.BoundsCenter = Center;
		D.BoundsExtent = Extent;
		D.MinAltitudeCm = MinZ;
		D.MaxAltitudeCm = MaxZ;
		D.AuthoredBuildingSlots = Slots;
		D.FlavorLine = FText::FromString(Flavor);
		RegisterDistrict(D);
	};

	// 12 districts across ~7 km x 7 km horizontal footprint (production-scale seed)
	AddDistrict(EMistspireWorldDistrict::ValleyHaven, TEXT("valley_haven"), FVector(0, 0, 0), FVector(350000, 350000, 200000), 0, 150000, 24, TEXT("Lantern smoke and rope bridges."));
	AddDistrict(EMistspireWorldDistrict::Mistmarket, TEXT("mistmarket"), FVector(700000, 0, 0), FVector(300000, 300000, 250000), 0, 350000, 18, TEXT("Bazaar tents vanish into white."));
	AddDistrict(EMistspireWorldDistrict::IronMesa, TEXT("iron_mesa"), FVector(0, 700000, 0), FVector(320000, 320000, 300000), 50000, 450000, 12, TEXT("Rust windmills on red stone."));
	AddDistrict(EMistspireWorldDistrict::CloudPromenade, TEXT("cloud_promenade"), FVector(700000, 700000, 0), FVector(280000, 280000, 350000), 200000, 550000, 10, TEXT("Bridges between floating gardens."));
	AddDistrict(EMistspireWorldDistrict::SpireUnderworks, TEXT("spire_underworks"), FVector(-500000, 300000, 0), FVector(250000, 250000, 400000), 0, 300000, 20, TEXT("Steam pipes and forgotten lifts."));
	AddDistrict(EMistspireWorldDistrict::RiftQuarter, TEXT("rift_quarter"), FVector(400000, -400000, 0), FVector(260000, 260000, 350000), 100000, 700000, 8, TEXT("Glass walkways over the void."));
	AddDistrict(EMistspireWorldDistrict::EmberSlums, TEXT("ember_slums"), FVector(-200000, -500000, 0), FVector(240000, 240000, 280000), 0, 400000, 16, TEXT("Coal heat, vertical alleys."));
	AddDistrict(EMistspireWorldDistrict::CathedralApproach, TEXT("cathedral_approach"), FVector(-300000, 400000, 0), FVector(220000, 220000, 500000), 300000, 900000, 6, TEXT("Bells below the crown of stone."));
	AddDistrict(EMistspireWorldDistrict::ObservatoryRing, TEXT("observatory_ring"), FVector(400000, 200000, 0), FVector(200000, 200000, 450000), 500000, 1100000, 5, TEXT("Telescopes chase the needle."));
	AddDistrict(EMistspireWorldDistrict::ZenithDock, TEXT("zenith_dock"), FVector(0, 0, 0), FVector(180000, 180000, 200000), 900000, 1400000, 4, TEXT("Airships moor at the roof of the world."));
	AddDistrict(EMistspireWorldDistrict::FrostArchive, TEXT("frost_archive"), FVector(-600000, -200000, 0), FVector(200000, 200000, 400000), 400000, 1000000, 7, TEXT("Ice vaults preserve dead maps."));
	AddDistrict(EMistspireWorldDistrict::StormBreak, TEXT("storm_break"), FVector(200000, 500000, 0), FVector(230000, 230000, 380000), 250000, 800000, 9, TEXT("Lightning scars the cliff face."));

	auto AddBuilding = [&](const TCHAR* Id, const TCHAR* District, FVector Door, FVector Interior, const TCHAR* Name, const TCHAR* EnterLine)
	{
		FMistspireBuildingEntry B;
		B.BuildingId = FName(Id);
		B.DistrictName = FName(District);
		B.ExteriorDoorLocation = Door;
		B.ExteriorDoorRotation = FRotator(0, 0, 0);
		B.InteriorSpawnLocation = Interior;
		B.InteriorSpawnRotation = FRotator::ZeroRotator;
		B.DisplayName = FText::FromString(Name);
		B.EnterLine = FText::FromString(EnterLine);
		B.bHasInterior = true;
		RegisterBuilding(B);
	};

	// Pocket interiors: offset +50 km per building index along X (designers mirror geometry at these coords)
	int32 PocketIndex = 0;
	auto PocketOffset = [&]() -> FVector
	{
		return FVector(5000000.f + PocketIndex++ * 800000.f, 0.f, 20000.f);
	};

	AddBuilding(TEXT("building_valley_inn"), TEXT("valley_haven"), FVector(12000, -8000, 18000), PocketOffset(), TEXT("Mist Inn"), TEXT("Warm air spills from the door."));
	AddBuilding(TEXT("building_valley_gear"), TEXT("valley_haven"), FVector(-15000, 22000, 17500), PocketOffset(), TEXT("Rope & Rivet"), TEXT("Climbing gear clinks inside."));
	AddBuilding(TEXT("building_mist_tea"), TEXT("mistmarket"), FVector(710000, 12000, 22000), PocketOffset(), TEXT("White Tea House"), TEXT("Steam masks the entrance."));
	AddBuilding(TEXT("building_mist_cartographer"), TEXT("mistmarket"), FVector(695000, -18000, 21500), PocketOffset(), TEXT("Cartographer's Shed"), TEXT("Maps rustle."));
	AddBuilding(TEXT("building_mesa_foundry"), TEXT("iron_mesa"), FVector(8000, 715000, 120000), PocketOffset(), TEXT("Mesa Foundry"), TEXT("Furnace heat washes out."));
	AddBuilding(TEXT("building_mesa_watch"), TEXT("iron_mesa"), FVector(-22000, 705000, 118000), PocketOffset(), TEXT("Watchtower"), TEXT("A narrow stair climbs inside."));
	AddBuilding(TEXT("building_cloud_salon"), TEXT("cloud_promenade"), FVector(705000, 710000, 280000), PocketOffset(), TEXT("Cloud Salon"), TEXT("Chimes echo in the hall."));
	AddBuilding(TEXT("building_under_lift"), TEXT("spire_underworks"), FVector(-490000, 310000, 45000), PocketOffset(), TEXT("Lift Control"), TEXT("Gears grind below street level."));
	AddBuilding(TEXT("building_under_archive"), TEXT("spire_underworks"), FVector(-520000, 280000, 42000), PocketOffset(), TEXT("Under-Archive"), TEXT("Dust and lamplight."));
	AddBuilding(TEXT("building_rift_glasshall"), TEXT("rift_quarter"), FVector(410000, -390000, 320000), PocketOffset(), TEXT("Glass Hall"), TEXT("The floor is sky."));
	AddBuilding(TEXT("building_ember_clinic"), TEXT("ember_slums"), FVector(-210000, -490000, 38000), PocketOffset(), TEXT("Ember Clinic"), TEXT("Antiseptic and ash."));
	AddBuilding(TEXT("building_cathedral_nave"), TEXT("cathedral_approach"), FVector(-310000, 410000, 420000), PocketOffset(), TEXT("Lower Nave"), TEXT("Incense rolls down the steps."));
	AddBuilding(TEXT("building_observatory_dome"), TEXT("observatory_ring"), FVector(405000, 210000, 620000), PocketOffset(), TEXT("Star Dome"), TEXT("Copper dome, cool inside."));
	AddBuilding(TEXT("building_zenith_hangar"), TEXT("zenith_dock"), FVector(5000, -5000, 920000), PocketOffset(), TEXT("Hangar 7"), TEXT("Hydrogen sweet and sharp."));
	AddBuilding(TEXT("building_frost_vault"), TEXT("frost_archive"), FVector(-610000, -190000, 480000), PocketOffset(), TEXT("Vault 3"), TEXT("Your breath freezes on the threshold."));
	AddBuilding(TEXT("building_storm_shelter"), TEXT("storm_break"), FVector(215000, 510000, 340000), PocketOffset(), TEXT("Storm Shelter"), TEXT("Concrete swallows sound."));

	auto AddPOI = [&](const TCHAR* Id, EMistspirePOIType Type, FVector Loc, const TCHAR* Title, const TCHAR* Desc)
	{
		FMistspirePOIEntry P;
		P.POIId = FName(Id);
		P.Type = Type;
		P.WorldLocation = Loc;
		P.Title = FText::FromString(Title);
		P.Description = FText::FromString(Desc);
		RegisterPOI(P);
	};

	AddPOI(TEXT("poi_valley_gate"), EMistspirePOIType::Landmark, FVector(0, 0, 20000), TEXT("Valley Gate"), TEXT("The climb begins here."));
	AddPOI(TEXT("poi_mist_falls"), EMistspirePOIType::Viewpoint, FVector(150000, 50000, 80000), TEXT("Mist Falls"), TEXT("Water vanishes into cloud."));
	AddPOI(TEXT("poi_mesa_wind"), EMistspirePOIType::Viewpoint, FVector(50000, 750000, 200000), TEXT("Mesa Wind Shrine"), TEXT("Flags snap in perpetual gale."));
	AddPOI(TEXT("poi_cloud_bridge"), EMistspirePOIType::Transit, FVector(720000, 680000, 350000), TEXT("Promenade Bridge"), TEXT("Cross between islands."));
	AddPOI(TEXT("poi_rift_crack"), EMistspirePOIType::Landmark, FVector(420000, -410000, 450000), TEXT("The Rift Crack"), TEXT("Stone peeled like fruit."));
	AddPOI(TEXT("poi_lore_first_ascent"), EMistspirePOIType::Lore, FVector(-10000, 30000, 50000), TEXT("First Ascent Plaque"), TEXT("They said the mist would never lift."));
	AddPOI(TEXT("poi_lore_failed_airship"), EMistspirePOIType::Lore, FVector(20000, -10000, 880000), TEXT("Hull Fragment"), TEXT("Zenith Dock rejected this vessel."));
}

void UMistspireWorldAtlasSubsystem::SpawnAuthoredWorldMarkers()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const FMistspireBuildingEntry& B : Buildings)
	{
		if (!B.bHasInterior)
		{
			continue;
		}

		AMistspireBuildingEntrance* Door = World->SpawnActor<AMistspireBuildingEntrance>(
			B.ExteriorDoorLocation, B.ExteriorDoorRotation, Params);
		if (Door)
		{
			Door->BuildingId = B.BuildingId;
		}

		const FVector ExitLoc = B.InteriorSpawnLocation + FVector(200.f, 0.f, 0.f);
		World->SpawnActor<AMistspireInteriorExit>(ExitLoc, FRotator::ZeroRotator, Params);
	}

	for (const FMistspirePOIEntry& P : POIs)
	{
		AMistspirePOIMarker* Marker = World->SpawnActor<AMistspirePOIMarker>(
			P.WorldLocation, FRotator::ZeroRotator, Params);
		if (Marker)
		{
			Marker->POIId = P.POIId;
			Marker->POIType = P.Type;
		}
	}
}
