#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MistspireHUD.generated.h"

/** Flat Play crosshair; hidden during VR Preview / HMD stereo. */
UCLASS()
class MISTSPIRE_API AMistspireHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
