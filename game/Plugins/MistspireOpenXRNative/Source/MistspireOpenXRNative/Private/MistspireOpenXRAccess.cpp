#include "MistspireOpenXRAccess.h"
#include "IOpenXRCore.h"
#include "Modules/ModuleManager.h"
bool FMistspireOpenXRAccess::IsOpenXRAvailable()
{
	const IOpenXRCore* C = FModuleManager::GetModulePtr<IOpenXRCore>(TEXT("OpenXRCore"));
	return C && C->IsTrackingInitialized();
}
bool FMistspireOpenXRAccess::GetNativeHandles(XrInstance& I, XrSession& S)
{
	I = S = nullptr;
	IOpenXRCore* C = FModuleManager::GetModulePtr<IOpenXRCore>(TEXT("OpenXRCore"));
	if (!C || !C->IsTrackingInitialized()) return false;
	I = C->GetInstance(); S = C->GetSession();
	return I && S;
}
