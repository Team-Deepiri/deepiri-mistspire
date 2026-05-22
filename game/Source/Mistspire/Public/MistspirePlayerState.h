#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MistspirePlayerState.generated.h"

UCLASS()
class MISTSPIRE_API AMistspirePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMistspirePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Score")
	float MaxAltitudeCm = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Score")
	float CurrentAltitudeCm = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mistspire|Score")
	TArray<FName> ReachedSummits;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Score")
	void UpdateAltitude(float NewAltitude);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Score")
	void AddSummit(FName SummitId);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SyncAltitudeToGameState(float Current, float Max);
};
