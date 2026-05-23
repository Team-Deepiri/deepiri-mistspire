#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MistspireSummitRegistry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMistspireSummitReached, FName, SummitId);

USTRUCT(BlueprintType)
struct FMistspireSummitEntry
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") FName SummitId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float OfficialAltitudeCm = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mistspire|Summit") float ReachRadiusCm = 500.f;
	UPROPERTY(BlueprintReadOnly, Category = "Mistspire|Summit") bool bReached = false;
};

UCLASS()
class MISTSPIRE_API UMistspireSummitRegistry : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Mistspire|Summit")
	void RegisterSummit(FName SummitId, FVector WorldLocation, float OfficialAltitudeCm, float ReachRadiusCm = 500.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Summit")
	bool TryReachSummit(FName SummitId, FVector PlayerLocation);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Summit")
	void ApplyReachedSummits(const TArray<FName>& SummitIds);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Summit")
	const TArray<FMistspireSummitEntry>& GetSummits() const { return Summits; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Summit")
	FName GetNextUnreachedSummitId(const TArray<FName>& AlreadyReached) const;

	UPROPERTY(BlueprintAssignable, Category = "Mistspire|Summit")
	FOnMistspireSummitReached OnSummitReached;

private:
	UPROPERTY() TArray<FMistspireSummitEntry> Summits;
};
