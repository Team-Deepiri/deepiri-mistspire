#pragma once

#include "CoreMinimal.h"

class UWorld;

/** Shared demo-presentation helpers for PIE / packaged owner recordings. */
namespace MistspireDemoMode
{
	/** True when mistspire.DemoMode=1 or -demoworld / -mistspiredemo on the command line. */
	bool IsEnabled();

	/** Enable HUD, welcome dialogue, wandering ghosts, and a clear-weather hold. */
	void ApplyPresentation(UWorld* World);

	/**
	 * Teleport the local pawn to a biome mid-band altitude (0–9) and optionally force visuals.
	 * Returns false if index is out of range or no pawn.
	 */
	bool TeleportToBiomeIndex(UWorld* World, int32 BiomeIndex, bool bForceVisuals = true);
}
