#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireXRActionSubsystem.generated.h"

struct XrActionSet_T; struct XrAction_T;
typedef struct XrActionSet_T* XrActionSet;
typedef struct XrAction_T* XrAction;

USTRUCT(BlueprintType)
struct FMistspireXRInputState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) bool bGrabLeft = false;
	UPROPERTY(BlueprintReadOnly) bool bGrabRight = false;
	UPROPERTY(BlueprintReadOnly) bool bClimbLeft = false;
	UPROPERTY(BlueprintReadOnly) bool bClimbRight = false;
	UPROPERTY(BlueprintReadOnly) bool bMenuPressed = false;
	UPROPERTY(BlueprintReadOnly) bool bJumpPressed = false;
	UPROPERTY(BlueprintReadOnly) bool bGrapplePressed = false;
	UPROPERTY(BlueprintReadOnly) bool bGliderPressed = false;
	UPROPERTY(BlueprintReadOnly) float MoveX = 0.f, MoveY = 0.f, Turn = 0.f;
};

UCLASS()
class MISTSPIREOPENXRNATIVE_API UMistspireXRActionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override;
	UFUNCTION(BlueprintCallable) void PollInputActions();
	UFUNCTION(BlueprintPure) const FMistspireXRInputState& GetInputState() const { return InputState; }
	UFUNCTION(BlueprintCallable) bool BuildActionLayout();

	/** Triggers haptic vibration on the specified hand. Amplitude 0-1, Frequency in Hz. */
	UFUNCTION(BlueprintCallable, Category = "Mistspire|XR")
	void TriggerHapticVibration(bool bIsLeftHand, float Amplitude, float DurationSeconds, float Frequency = 60.f);

private:
	XrActionSet ActionSet = nullptr;
	XrAction GrabAction = nullptr, MoveAction = nullptr, StrafeAction = nullptr, TurnAction = nullptr;
	XrAction JumpAction = nullptr, ClimbAction = nullptr, MenuAction = nullptr;
	XrAction GrappleAction = nullptr, GliderAction = nullptr;
	XrAction HapticAction = nullptr;
	FMistspireXRInputState InputState;
	bool bActionsReady = false;
	static void CopyOpenXRString(char* Dest, size_t DestSize, const char* Src);
	bool AttachActionSetToSession();
};
