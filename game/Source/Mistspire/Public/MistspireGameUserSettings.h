#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "MistspireGameUserSettings.generated.h"

class AMistspireVRPawn;

/** Persisted non-VR display / feel settings (Esc menu). */
UCLASS(config = GameUserSettings, configdonotcheckdefaults)
class MISTSPIRE_API UMistspireGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UMistspireGameUserSettings();

	static UMistspireGameUserSettings* Get();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	float GetMouseSensitivity() const { return MouseSensitivity; }

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	void SetMouseSensitivity(float Value);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	bool IsViewBobbingEnabled() const { return bViewBobbing; }

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	void SetViewBobbingEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	float GetViewBobScale() const { return ViewBobScale; }

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	void SetViewBobScale(float Value);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	float GetFieldOfView() const { return FieldOfView; }

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	void SetFieldOfView(float Degrees);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	bool IsControlsHintEnabled() const { return bShowControlsHint; }

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Settings")
	void SetControlsHintEnabled(bool bEnabled);

	/** Apply FOV (and related) to the local non-VR pawn. */
	void ApplyGameplaySettings(AMistspireVRPawn* Pawn);

	/** Cycle Windowed -> Borderless -> Fullscreen -> Windowed and apply. */
	void CycleFullscreenMode();

	FText GetFullscreenModeLabel() const;

protected:
	UPROPERTY(config)
	float MouseSensitivity = 1.f;

	UPROPERTY(config)
	bool bViewBobbing = true;

	UPROPERTY(config)
	float ViewBobScale = 1.f;

	UPROPERTY(config)
	float FieldOfView = 90.f;

	/** On-screen WASD / key reference; off by default. */
	UPROPERTY(config)
	bool bShowControlsHint = false;
};
