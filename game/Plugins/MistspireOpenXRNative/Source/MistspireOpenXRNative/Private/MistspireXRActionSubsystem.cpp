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
	XrActionSetCreateInfo SCI{XR_TYPE_ACTION_SET_CREATE_INFO};
	CopyOpenXRString(SCI.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "mistspire_gameplay");
	CopyOpenXRString(SCI.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "Mistspire Gameplay");
	if (XR_FAILED(xrCreateActionSet(I, &SCI, &ActionSet))) return false;
	auto Mk = [&](XrActionType T, const char* N, const char* L, XrAction& O) {
		XrActionCreateInfo CI{XR_TYPE_ACTION_CREATE_INFO}; CI.actionType = T;
		CopyOpenXRString(CI.actionName, XR_MAX_ACTION_NAME_SIZE, N);
		CopyOpenXRString(CI.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, L);
		return XR_SUCCEEDED(xrCreateAction(ActionSet, &CI, &O));
	};
	if (!Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "grab", "Grab", GrabAction) || !Mk(XR_ACTION_TYPE_FLOAT_INPUT, "move", "Move", MoveAction) ||
		!Mk(XR_ACTION_TYPE_FLOAT_INPUT, "strafe", "Strafe", StrafeAction) ||
		!Mk(XR_ACTION_TYPE_FLOAT_INPUT, "turn", "Turn", TurnAction) || !Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "jump", "Jump", JumpAction) ||
		!Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "climb", "Climb", ClimbAction) || !Mk(XR_ACTION_TYPE_BOOLEAN_INPUT, "menu", "Menu", MenuAction))
		return false;
	bActionsReady = AttachActionSetToSession();
	return bActionsReady;
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
	XrActiveActionSet AS{ActionSet, XR_NULL_PATH};
	XrActionsSyncInfo SI{XR_TYPE_ACTIONS_SYNC_INFO}; SI.countActiveActionSets = 1; SI.activeActionSets = &AS;
	if (XR_FAILED(xrSyncActions(S, &SI))) return;
	auto RB = [&](XrAction A, bool& V) {
		XrActionStateGetInfo GI{XR_TYPE_ACTION_STATE_GET_INFO}; GI.action = A;
		XrActionStateBoolean ST{XR_TYPE_ACTION_STATE_BOOLEAN};
		if (XR_SUCCEEDED(xrGetActionStateBoolean(S, &GI, &ST)) && ST.isActive) V = ST.currentState != 0;
	};
	auto RF = [&](XrAction A, float& V) {
		XrActionStateGetInfo GI{XR_TYPE_ACTION_STATE_GET_INFO}; GI.action = A;
		XrActionStateFloat ST{XR_TYPE_ACTION_STATE_FLOAT};
		if (XR_SUCCEEDED(xrGetActionStateFloat(S, &GI, &ST)) && ST.isActive) V = ST.currentState;
	};
	RB(GrabAction, InputState.bGrabPressed);
	RB(MenuAction, InputState.bMenuPressed);
	RB(JumpAction, InputState.bJumpPressed);
	RB(ClimbAction, InputState.bClimbPressed);
	RF(MoveAction, InputState.MoveY);
	RF(StrafeAction, InputState.MoveX);
	RF(TurnAction, InputState.Turn);
}
