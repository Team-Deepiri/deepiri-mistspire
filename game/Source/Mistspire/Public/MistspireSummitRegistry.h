#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireSummitRegistry.generated.h"

USTRUCT(BlueprintType)
struct FMistspireSummitEntry
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") FName SummitId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float OfficialAltitudeCm = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Summit") bool bReached = false;
};

UCLASS()
class MISTSPIRE_API UMistspireSummitRegistry : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Summit")
	void RegisterSummit(FName SummitId, FVector WorldLocation, float OfficialAltitudeCm);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Summit")
	bool TryReachSummit(FName SummitId, FVector PlayerLocation, float ReachRadiusCm = 500.f);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Summit")
	const TArray<FMistspireSummitEntry>& GetSummits() const { return Summits; }

private:
	UPROPERTY() TArray<FMistspireSummitEntry> Summits;
};
