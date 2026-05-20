#pragma once
#include "Modules/ModuleManager.h"
class FMistspireOpenXRNativeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
