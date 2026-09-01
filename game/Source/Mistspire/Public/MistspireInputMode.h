#pragma once

#include "CoreMinimal.h"

class UWorld;

/** Shared detection for keyboard/mouse (non-VR) vs OpenXR play. */
struct MISTSPIRE_API FMistspireInputMode
{
	static bool IsNonVRMode(const UWorld* World);

	/** On-screen / toast hint for non-VR controls (single source of truth). */
	static const TCHAR* GetNonVRControlsHint();

	/** Register WASD/mouse legacy mappings at runtime (survives editor ini overwrites). */
	static void EnsureLegacyNonVRKeyMappings();
};
