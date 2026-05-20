#pragma once
#include "CoreMinimal.h"
struct XrInstance_T; struct XrSession_T;
typedef struct XrInstance_T* XrInstance;
typedef struct XrSession_T* XrSession;
class MISTSPIREOPENXRNATIVE_API FMistspireOpenXRAccess
{
public:
	static bool IsOpenXRAvailable();
	static bool GetNativeHandles(XrInstance& OutInstance, XrSession& OutSession);
};
