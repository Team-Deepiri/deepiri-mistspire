#pragma once

#include "CoreMinimal.h"

class UWorld;

/** Shared detection for keyboard/mouse (non-VR) vs OpenXR play. */
struct MISTSPIRE_API FMistspireInputMode
{
	static bool IsNonVRMode(const UWorld* World);

	/** @deprecated Use IsNonVRMode */
	static bool IsNonVRMode() { return IsNonVRMode(nullptr); }
};
