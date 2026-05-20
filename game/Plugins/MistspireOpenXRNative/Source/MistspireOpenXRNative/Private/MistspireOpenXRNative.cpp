#include "MistspireOpenXRNative.h"

#define LOCTEXT_NAMESPACE "MistspireOpenXRNative"

void FMistspireOpenXRNativeModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("MistspireOpenXRNative loaded — action set: mistspire_gameplay"));
}

void FMistspireOpenXRNativeModule::ShutdownModule() {}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMistspireOpenXRNativeModule, MistspireOpenXRNative)
