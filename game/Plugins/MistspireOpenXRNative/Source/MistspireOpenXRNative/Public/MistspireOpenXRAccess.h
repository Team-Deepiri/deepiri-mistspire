#pragma once
#include "CoreMinimal.h"
#include <openxr/openxr.h>

class MISTSPIREOPENXRNATIVE_API FMistspireOpenXRAccess
{
public:
	/** True when UE OpenXRHMD has an initialized session we can read. */
	static bool IsOpenXRAvailable();

	/**
	 * Reads native OpenXR handles from UE's OpenXRHMD (never creates a parallel instance/session).
	 * @return true when both instance and session are non-null.
	 */
	static bool GetNativeHandles(XrInstance& OutInstance, XrSession& OutSession);
};
