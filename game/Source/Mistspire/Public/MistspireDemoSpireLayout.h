#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"

/**
 * Shared Demo Spire helix layout — stations, summit IDs, and tints.
 * Used by summit seeds, DemoTour teleports, and AMistspireDemoClimbScaffold.
 *
 * ValleyOrigin is the mountain-top shelf where the gate, Mist Inn, spawn, and helix root live.
 * Station altitudes are measured upward from that shelf.
 */
namespace MistspireDemoSpire
{
	inline constexpr int32 StationCount = 10;
	inline constexpr float HelixRadiusCm = 2500.f;
	inline constexpr float AngleStepDeg = 36.f;

	/** Summit shelf world origin — helix + gate village root (near map floor so mountain can hide the template). */
	inline FVector GetValleyOrigin()
	{
		return FVector(0.f, 0.f, 50.f);
	}

	inline FVector Valley(float X, float Y, float Z)
	{
		return GetValleyOrigin() + FVector(X, Y, Z);
	}

	inline float GetValleyFloorZCm()
	{
		return GetValleyOrigin().Z;
	}

	/** Capsule clearance above shelf walk surface. */
	inline constexpr float PlayerStartZCm = 120.f;

	/**
	 * Spawn in front of Valley Gate, looking toward the arch (-X / yaw 180).
	 * Gate sits between spawn and the mast so the mountain is the backdrop, not the gate's backstop.
	 */
	inline FVector GetValleySpawnLocation()
	{
		return Valley(850.f, 0.f, PlayerStartZCm);
	}

	/** Gate arch center on the shelf (between spawn and mast). */
	inline FVector GetValleyGateLocation()
	{
		return Valley(400.f, 0.f, 0.f);
	}

	/** Local station stair run height (12 × 40 cm). */
	inline constexpr float VignetteHeightCm = 480.f;
	/** Approach stair rise — must stay ≤ NonVRMaxStepHeightCm (45). */
	inline constexpr float ApproachStepZCm = 40.f;
	/** End of walkable approach helix; grapple highway continues to Mist. */
	inline constexpr float ApproachEndZCm = 6000.f;
	/** Helix angular sweep (degrees) so consecutive 40 cm stairs have ≥90 cm tread. */
	inline constexpr float ApproachSweepDeg = 450.f;
	/** Capsule half-height (~88) + margin for DemoTour landings. */
	inline constexpr float TourLandingClearanceCm = 120.f;
	/**
	 * DemoTour lands this far toward the mast instead of on the pad centre. Needle, plinth and
	 * pier silhouettes occupy the centre / outer half of their decks, and a bSweep=false teleport
	 * into one leaves the capsule depenetrating inside solid geometry.
	 */
	inline constexpr float TourLandingInsetCm = 140.f;

	/** Pad half-extent used for dress occupancy (cm). Dress stays outside this + margin. */
	inline constexpr float StationPadHalfCm = 300.f;
	inline constexpr float StationDressRingCm = 520.f;

	/**
	 * Authored Main_WP `DemoEnv_*` / Iceland tile clutter outside this XY radius from the valley
	 * is destroyed at runtime (SummitMass keep zone ≈ 1.5 km half-extent).
	 */
	inline constexpr float LegacyDressKeepRadiusCm = 160000.f;
	/** Only purge low-altitude map dress — helix stations and the spire stay untouched. */
	inline constexpr float LegacyDressPurgeMaxAltitudeCm = 150000.f;

	inline constexpr float StationAltitudeCm[StationCount] = {
		50000.f,   // Mist
		200000.f,  // Arid
		400000.f,  // Forest
		600000.f,  // Ember
		800000.f,  // Crystal
		1050000.f, // Void
		1300000.f, // Tundra
		1500000.f, // Aether
		1700000.f, // Sanctum
		1900000.f  // Pinnacle
	};

	inline const TCHAR* SummitIds[StationCount] = {
		TEXT("summit_valley_gate"),
		TEXT("summit_mesa_crown"),
		TEXT("summit_cloud_garden"),
		TEXT("summit_ember_crown"),
		TEXT("summit_rift_observatory"),
		TEXT("summit_spire_cathedral"),
		TEXT("summit_obelisk_prime"),
		TEXT("summit_aether_span"),
		TEXT("summit_sanctum_crown"),
		TEXT("summit_orbital_needle")
	};

	inline const TCHAR* BiomeNames[StationCount] = {
		TEXT("Mist"), TEXT("Arid"), TEXT("Forest"), TEXT("Ember"), TEXT("Crystal"),
		TEXT("Void"), TEXT("Tundra"), TEXT("Aether"), TEXT("Sanctum"), TEXT("Pinnacle")
	};

	inline float GetStationYawDeg(int32 Index)
	{
		return static_cast<float>(Index) * AngleStepDeg;
	}

	inline FVector GetStationLocation(int32 Index)
	{
		if (Index < 0 || Index >= StationCount)
		{
			return GetValleyOrigin();
		}
		const float AngleRad = FMath::DegreesToRadians(GetStationYawDeg(Index));
		return Valley(
			HelixRadiusCm * FMath::Cos(AngleRad),
			HelixRadiusCm * FMath::Sin(AngleRad),
			StationAltitudeCm[Index]);
	}

	/** Safe DemoTour / teleport landing: above pad top, inset toward the mast, clear of dress. */
	inline FVector GetTourLandingLocation(int32 Index)
	{
		const FVector Station = GetStationLocation(Index);
		if (Index < 0 || Index >= StationCount)
		{
			return Station + FVector(0.f, 0.f, TourLandingClearanceCm);
		}

		const float AngleRad = FMath::DegreesToRadians(GetStationYawDeg(Index));
		const FVector RadialOut(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f);
		return Station - RadialOut * TourLandingInsetCm + FVector(0.f, 0.f, TourLandingClearanceCm);
	}

	/**
	 * Mist Inn door — off the -Y side of the plaza, facing spawn (+X).
	 * Kept clear of the gate: solid dress is refused inside InnDoorClearanceCm.
	 */
	inline FVector GetMistInnDoorLocation()
	{
		return Valley(400.f, -1700.f, 80.f);
	}

	/** Collision dress must stay outside this radius around the inn door. */
	inline constexpr float InnDoorClearanceCm = 700.f;

	/**
	 * Enter-volume sits on the plaza side of the wall so the capsule overlaps it before
	 * reaching the visual opening (cabin shell is NoCollision; teleport is the interaction).
	 */
	inline FVector GetMistInnDoorTriggerLocation()
	{
		// FacePlaza forward is +Y toward the plaza/gate from the -Y wing.
		return GetMistInnDoorLocation() + FVector(0.f, 120.f, 0.f);
	}

	/**
	 * Where ExitBuilding should drop the pawn — shelf floor + capsule clearance on the plaza
	 * side of the door. Must NOT use the door-volume center Z (that left you floating above the porch).
	 */
	inline FVector GetMistInnExitReturnLocation()
	{
		const FVector Door = GetMistInnDoorLocation();
		// Past the enter volume (trigger at Door+120Y, extent ~180 local → world X after yaw 90).
		// Z here is only a fallback — InteriorSubsystem overwrites it with the porch stance Z.
		return FVector(Door.X, Door.Y + 450.f, GetValleyFloorZCm() + PlayerStartZCm);
	}

	/**
	 * Mist Inn pocket interior origin (matches atlas building_valley_inn first pocket).
	 * This is the pocket FLOOR level; geometry is runtime-built by AMistspireDemoClimbScaffold.
	 */
	inline FVector GetMistInnInteriorSpawn()
	{
		return FVector(5000000.f, 0.f, 20000.f);
	}

	/**
	 * Where the pawn lands inside the pocket. Using the floor level directly put the capsule
	 * centre on the floor plane, so half the capsule started inside the slab — depenetration
	 * either popped the player or dropped them through into a 200 m fall.
	 */
	inline FVector GetMistInnInteriorPawnSpawn()
	{
		return GetMistInnInteriorSpawn() + FVector(0.f, 0.f, PlayerStartZCm);
	}

	/** InteriorExit volume — centered in the pocket's +X door wall opening. */
	inline FVector GetMistInnInteriorExitLocation()
	{
		return GetMistInnInteriorSpawn() + FVector(400.f, 0.f, 100.f);
	}

	inline FName GetSummitId(int32 Index)
	{
		if (Index < 0 || Index >= StationCount)
		{
			return NAME_None;
		}
		return FName(SummitIds[Index]);
	}

	inline FLinearColor GetBiomeTint(int32 Index)
	{
		switch (Index)
		{
		case 0: return FLinearColor(0.45f, 0.55f, 0.70f); // Mist cool grey-blue
		case 1: return FLinearColor(0.72f, 0.45f, 0.28f); // Arid rust-ochre
		case 2: return FLinearColor(0.28f, 0.52f, 0.32f); // Forest green
		case 3: return FLinearColor(0.42f, 0.22f, 0.16f); // Ember readable charcoal-rust
		case 4: return FLinearColor(0.35f, 0.75f, 0.85f); // Crystal teal
		case 5: return FLinearColor(0.28f, 0.22f, 0.42f); // Void purple-grey (readable)
		case 6: return FLinearColor(0.78f, 0.85f, 0.92f); // Tundra ice
		case 7: return FLinearColor(0.62f, 0.52f, 0.82f); // Aether lilac
		case 8: return FLinearColor(0.82f, 0.68f, 0.35f); // Sanctum gold
		case 9: return FLinearColor(0.85f, 0.92f, 1.00f); // Pinnacle cyan-white
		default: return FLinearColor::Gray;
		}
	}

	inline FLinearColor GetBiomeLightColor(int32 Index)
	{
		switch (Index)
		{
		case 0: return FLinearColor(0.35f, 0.55f, 1.0f);
		case 1: return FLinearColor(1.0f, 0.55f, 0.25f);
		case 2: return FLinearColor(0.35f, 0.85f, 0.40f);
		case 3: return FLinearColor(1.0f, 0.35f, 0.08f);
		case 4: return FLinearColor(0.25f, 0.90f, 1.0f);
		case 5: return FLinearColor(0.55f, 0.20f, 0.90f);
		case 6: return FLinearColor(0.70f, 0.85f, 1.0f);
		case 7: return FLinearColor(0.75f, 0.55f, 1.0f);
		case 8: return FLinearColor(1.0f, 0.80f, 0.35f);
		case 9: return FLinearColor(0.85f, 0.95f, 1.0f);
		default: return FLinearColor::White;
		}
	}
}
