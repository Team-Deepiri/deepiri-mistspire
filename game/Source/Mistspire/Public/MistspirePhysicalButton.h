#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MistspirePhysicalButton.generated.h"

class UStaticMeshComponent;
class UPhysicsConstraintComponent;

UENUM(BlueprintType)
enum class EMistspireButtonAction : uint8
{
	None,
	CycleWeather,
	RefillSurvival,
	TeleportUp
};

UCLASS()
class MISTSPIRE_API AMistspirePhysicalButton : public AActor
{
	GENERATED_BODY()

public:
	AMistspirePhysicalButton();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressed);
	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Interaction")
	FOnButtonPressed OnButtonPressed;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Components")
	TObjectPtr<UStaticMeshComponent> ButtonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mistspire|Components")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Interaction")
	float ActivationThresholdCm = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Interaction")
	EMistspireButtonAction BuiltInAction = EMistspireButtonAction::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Interaction")
	float TeleportUpCm = 3000.f;

private:
	void ExecuteBuiltInAction();
	bool bIsPressed = false;
	FVector InitialLocalLocation;
};
