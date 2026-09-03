#pragma once

#include "CoreMinimal.h"

class APlayerController;
class UInputAction;
class UInputMappingContext;
class UWorld;

/** Runtime Enhanced Input assets for keyboard/mouse (non-VR). Owned by the pawn. */
struct MISTSPIRE_API FMistspireNonVREnhancedInput
{
	TObjectPtr<UInputMappingContext> MappingContext;
	TObjectPtr<UInputAction> Move;
	TObjectPtr<UInputAction> Look;
	TObjectPtr<UInputAction> Jump;
	TObjectPtr<UInputAction> Climb;
	TObjectPtr<UInputAction> Sprint;
	TObjectPtr<UInputAction> Grapple;
	TObjectPtr<UInputAction> Glider;
	TObjectPtr<UInputAction> Teleport;
	TObjectPtr<UInputAction> Interact;

	bool IsValid() const { return MappingContext && Move && Look && Jump && Climb && Sprint && Grapple && Glider && Teleport && Interact; }
};

/** Shared detection for keyboard/mouse (non-VR) vs OpenXR play. */
struct MISTSPIRE_API FMistspireInputMode
{
	static bool IsNonVRMode(const UWorld* World);

	/** On-screen / toast hint for non-VR controls (single source of truth). */
	static const TCHAR* GetNonVRControlsHint();

	/** Build WASD/mouse Enhanced Input actions + mapping context (no Content assets). */
	static void CreateNonVREnhancedInput(UObject* Outer, FMistspireNonVREnhancedInput& Out);

	/** Register the non-VR mapping context on the local player. */
	static void AddNonVRMappingContext(APlayerController* PlayerController, const UInputMappingContext* MappingContext);
};
