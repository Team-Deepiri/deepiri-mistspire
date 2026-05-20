#include "MistspireXRActionSubsystem.h"
#include "MistspireOpenXRAccess.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include <openxr/openxr.h>

void UMistspireXRActionSubsystem::CopyOpenXRString(char* Dest, size_t DestSize, const char* Src)
{
	if (!Dest || !DestSize) return;
	FCStringAnsi::Strncpy(Dest, Src, static_cast<int32>(DestSize));
	Dest[DestSize - 1] = '\0';
}

void UMistspireXRActionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UMistspireXRActionSubsystem::BuildActionLayout));
}

void UMistspireXRActionSubsystem::Deinitialize()
{
	XrInstance I = nullptr; XrSession S = nullptr;
	if (FMistspireOpenXRAccess::GetNativeHandles(I, S) && ActionSet && I) xrDestroyActionSet(ActionSet);
	ActionSet = nullptr;
	GrabAction = MoveAction = StrafeAction = TurnAction = JumpAction = ClimbAction = MenuAction = nullptr;
	bActionsReady = false;
	Super::Deinitialize();
}

TStatId UMistspireXRActionSubsystem::GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireXRActionSubsystem, STATGROUP_Tickables); }
void UMistspireXRActionSubsystem::Tick(float) { PollInputActions(); }

bool UMistspireXRActionSubsystem::BuildActionLayout()
{
	if (bActionsReady) return true;
	XrInstance I = nullptr; XrSession S = nullptr;
	if (!FMistspireOpenXRAccess::GetNativeHandles(I, S)) return false;

	XrPath LeftHandPath, RightHandPath;
	xrStringToPath(I, "/user/hand/left", &LeftHandPath);
	xrStringToPath(I, "/user/hand/right", &RightHandPath);
	XrPath HandPaths[] = { LeftHandPath, RightHandPath };

	XrActionSetCreateInfo SCI{XR_TYPE_ACTION_SET_CREATE_INFO};
	CopyOpenXRString(SCI.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "mistspire_gameplay");
	CopyOpenXRString(SCI.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "Mistspire Gameplay");
	if (XR_FAILED(xrCreateActionSet(I, &SCI, &ActionSet))) return false;

	auto Mk = [&](XrActionType T, const char* N, const char* L, XrAction& O, bool bPerHand) {
		XrActionCreateInfo CI{XR_TYPE_ACTION_CREATE_INFO}; CI.actionType = T;
		CopyOpenXRString(CI.actionName, XR_MAX_ACTION_NAME_SIZE, N);
		CopyOpenXRString(CI.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, L);
		if (bPerHand) {
			CI.countSubactionPaths = 2;
			CI.subactionPaths = HandPaths;
		}
		return XR_SUCCEEDED(xrCreateAction(ActionSet, &CI, &O));
	};

	if (!Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "grab", "Grab", GrabAction, true) ||
		!Mk(XR_ACTION_TYPE_FLOAT_INPUT, "move", "Move", MoveAction, false) ||
		!Mk(XR_ACTION_TYPE_FLOAT_INPUT, "strafe", "Strafe", StrafeAction, false) ||
		!Mk(XR_ACTION_TYPE_FLOAT_INPUT, "turn", "Turn", TurnAction, false) ||
		!Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "jump", "Jump", JumpAction, false) ||
		!Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "climb", "Climb", ClimbAction, true) ||
		!Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "menu", "Menu", MenuAction, false) ||
		!Mk(XR_ACTION_TYPE_VIBRATION_OUTPUT, "haptic", "Haptic", HapticAction, true))
		return false;

	bActionsReady = AttachActionSetToSession();
	return bActionsReady;
}

void UMistspireXRActionSubsystem::TriggerHapticVibration(bool bIsLeftHand, float Amplitude, float DurationSeconds, float Frequency)
{
	XrInstance I = nullptr; XrSession S = nullptr;
	if (!FMistspireOpenXRAccess::GetNativeHandles(I, S) || !HapticAction) return;

	XrPath HandPath;
	xrStringToPath(I, bIsLeftHand ? "/user/hand/left" : "/user/hand/right", &HandPath);

	XrHapticVibration Vibration{XR_TYPE_HAPTIC_VIBRATION};
	Vibration.amplitude = FMath::Clamp(Amplitude, 0.f, 1.f);
	Vibration.duration = static_cast<XrDuration>(DurationSeconds * 1e9); // Seconds to nanoseconds
	Vibration.frequency = Frequency;

	XrHapticActionInfo HI{XR_TYPE_HAPTIC_ACTION_INFO};
	HI.action = HapticAction;
	HI.subactionPath = HandPath;

	xrApplyHapticFeedback(S, &HI, reinterpret_cast<const XrHapticBaseHeader*>(&Vibration));
}

bool UMistspireXRActionSubsystem::AttachActionSetToSession()
{
	XrInstance I = nullptr; XrSession S = nullptr;
	if (!FMistspireOpenXRAccess::GetNativeHandles(I, S) || !ActionSet) return false;
	XrSessionActionSetsAttachInfo AI{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
	AI.countActionSets = 1; AI.actionSets = &ActionSet;
	return XR_SUCCEEDED(xrAttachSessionActionSets(S, &AI));
}

void UMistspireXRActionSubsystem::PollInputActions()
{
	if (!bActionsReady && !BuildActionLayout()) return;
	XrInstance I = nullptr; XrSession S = nullptr;
	if (!FMistspireOpenXRAccess::GetNativeHandles(I, S)) return;

	XrPath LeftHandPath, RightHandPath;
	xrStringToPath(I, "/user/hand/left", &LeftHandPath);
	xrStringToPath(I, "/user/hand/right", &RightHandPath);

	XrActiveActionSet AS{ActionSet, XR_NULL_PATH};
	XrActionsSyncInfo SI{XR_TYPE_ACTIONS_SYNC_INFO}; SI.countActiveActionSets = 1; SI.activeActionSets = &AS;
	if (XR_FAILED(xrSyncActions(S, &SI))) return;

	auto RB = [&](XrAction A, XrPath P, bool& V) {
		XrActionStateGetInfo GI{XR_TYPE_ACTION_STATE_GET_INFO}; GI.action = A; GI.subactionPath = P;
		XrActionStateBoolean ST{XR_TYPE_ACTION_STATE_BOOLEAN};
		if (XR_SUCCEEDED(xrGetActionStateBoolean(S, &GI, &ST)) && ST.isActive) V = ST.currentState != 0;
	};
	auto RF = [&](XrAction A, XrPath P, float& V) {
		XrActionStateGetInfo GI{XR_TYPE_ACTION_STATE_GET_INFO}; GI.action = A; GI.subactionPath = P;
		XrActionStateFloat ST{XR_TYPE_ACTION_STATE_FLOAT};
		if (XR_SUCCEEDED(xrGetActionStateFloat(S, &GI, &ST)) && ST.isActive) V = ST.currentState;
	};

	RB(GrabAction, LeftHandPath, InputState.bGrabLeft);
	RB(GrabAction, RightHandPath, InputState.bGrabRight);
	RB(ClimbAction, LeftHandPath, InputState.bClimbLeft);
	RB(ClimbAction, RightHandPath, InputState.bClimbRight);
	
	RB(MenuAction, XR_NULL_PATH, InputState.bMenuPressed);
	RB(JumpAction, XR_NULL_PATH, InputState.bJumpPressed);
	
	RF(MoveAction, XR_NULL_PATH, InputState.MoveY);
	RF(StrafeAction, XR_NULL_PATH, InputState.MoveX);
	RF(TurnAction, XR_NULL_PATH, InputState.Turn);
}
