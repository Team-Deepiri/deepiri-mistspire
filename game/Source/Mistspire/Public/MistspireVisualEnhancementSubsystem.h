#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Systems/MistspireBiomeSubsystem.h"
#include "MistspireVisualEnhancementSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMistspireBiomeVisuals
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BloomIntensity = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BloomThreshold = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AmbientOcclusionIntensity = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VolumetricFogDensity = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VolumetricFogAlbedo = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor VolumetricFogColor = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorTemperature = 6500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorTintShadows = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Saturation = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Contrast = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Gamma = 2.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowBrightness = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HighlightBrightness = 1.0f;
};

UCLASS()
class MISTSPIRE_API UMistspireVisualEnhancementSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintPure, Category = "Mistspire|Visuals")
	FMistspireBiomeVisuals GetCurrentVisuals() const { return CurrentVisuals; }

	UFUNCTION(BlueprintPure, Category = "Mistspire|Visuals")
	EMistspireBiomeType GetCurrentBiome() const { return CurrentBiome; }

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Visuals")
	void ForceBiomeVisuals(EMistspireBiomeType Biome);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Visuals")
	void ClearForcedBiome();

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Visuals")
	void SetOverrideIntensity(float Intensity) { OverrideIntensity = FMath::Clamp(Intensity, 0.f, 2.f); }

	float GetOverrideIntensity() const { return OverrideIntensity; }

private:
	void UpdateCurrentBiome();
	void ApplyCurrentVisuals();
	void ApplyBloom(float Intensity, float Threshold);
	void ApplyVolumetricFog(float Density, float Albedo, const FLinearColor& Color);
	void ApplyColorGrading(float Temperature, float Tint, float Saturation, float Contrast, float Gamma);
	void ApplyAO(float Intensity);
	void TransitionVisuals(float DeltaTime);

	static FMistspireBiomeVisuals MakeBiomeVisuals(EMistspireBiomeType Biome);

	EMistspireBiomeType CurrentBiome = EMistspireBiomeType::None;
	FMistspireBiomeVisuals TargetVisuals;
	FMistspireBiomeVisuals CurrentVisuals;
	FMistspireBiomeVisuals PreviousVisuals;

	bool bForced = false;
	EMistspireBiomeType ForcedBiome = EMistspireBiomeType::None;
	float TransitionAlpha = 1.f;
	float OverrideIntensity = 1.f;
	float TransitionDuration = 3.f;
	float ElapsedTime = 0.f;
};
