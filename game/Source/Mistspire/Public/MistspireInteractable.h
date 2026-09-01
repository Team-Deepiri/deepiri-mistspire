#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MistspireInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UMistspireInteractable : public UInterface
{
	GENERATED_BODY()
};

class MISTSPIRE_API IMistspireInteractable
{
	GENERATED_BODY()

public:
	/** Keyboard/mouse Use (E) or VR proximity activate. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mistspire|Interaction")
	void MistspireInteract(AActor* InteractInstigator);
};
