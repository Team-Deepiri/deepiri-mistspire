#include "MistspireVisualEnhancementSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UMistspireVisualEnhancementSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CurrentBiome = EMistspireBiomeType::Mist;
	CurrentVisuals = MakeBiomeVisuals(CurrentBiome);
	TargetVisuals = CurrentVisuals;
	PreviousVisuals = CurrentVisuals;
	TransitionAlpha = 1.f;
	ApplyCurrentVisuals();
}

void UMistspireVisualEnhancementSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCurrentBiome();
	TransitionVisuals(DeltaTime);
	ApplyCurrentVisuals();
}

TStatId UMistspireVisualEnhancementSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireVisualEnhancementSubsystem, STATGROUP_Tickables);
}

void UMistspireVisualEnhancementSubsystem::UpdateCurrentBiome()
{
	EMistspireBiomeType Detected = EMistspireBiomeType::Mist;
	if (UWorld* World = GetWorld())
	{
		if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			Detected = Env->GetCurrentBiome();
		}
	}

	EMistspireBiomeType NewBiome = bForced ? ForcedBiome : Detected;
	if (NewBiome != CurrentBiome)
	{
		PreviousVisuals = CurrentVisuals;
		CurrentBiome = NewBiome;
		TargetVisuals = MakeBiomeVisuals(CurrentBiome);
		TransitionAlpha = 0.f;
		ElapsedTime = 0.f;
	}
}

void UMistspireVisualEnhancementSubsystem::TransitionVisuals(float DeltaTime)
{
	if (TransitionAlpha < 1.f)
	{
		ElapsedTime += DeltaTime;
		TransitionAlpha = FMath::Min(ElapsedTime / TransitionDuration, 1.f);

		float Smooth = FMath::InterpEaseInOut(0.f, 1.f, TransitionAlpha, 2.f);
		CurrentVisuals.BloomIntensity = FMath::Lerp(PreviousVisuals.BloomIntensity, TargetVisuals.BloomIntensity, Smooth);
		CurrentVisuals.BloomThreshold = FMath::Lerp(PreviousVisuals.BloomThreshold, TargetVisuals.BloomThreshold, Smooth);
		CurrentVisuals.AmbientOcclusionIntensity = FMath::Lerp(PreviousVisuals.AmbientOcclusionIntensity, TargetVisuals.AmbientOcclusionIntensity, Smooth);
		CurrentVisuals.VolumetricFogDensity = FMath::Lerp(PreviousVisuals.VolumetricFogDensity, TargetVisuals.VolumetricFogDensity, Smooth);
		CurrentVisuals.VolumetricFogAlbedo = FMath::Lerp(PreviousVisuals.VolumetricFogAlbedo, TargetVisuals.VolumetricFogAlbedo, Smooth);
		CurrentVisuals.VolumetricFogColor = FLinearColor::LerpUsingHSV(PreviousVisuals.VolumetricFogColor, TargetVisuals.VolumetricFogColor, Smooth);
		CurrentVisuals.ColorTemperature = FMath::Lerp(PreviousVisuals.ColorTemperature, TargetVisuals.ColorTemperature, Smooth);
		CurrentVisuals.ColorTintShadows = FMath::Lerp(PreviousVisuals.ColorTintShadows, TargetVisuals.ColorTintShadows, Smooth);
		CurrentVisuals.Saturation = FMath::Lerp(PreviousVisuals.Saturation, TargetVisuals.Saturation, Smooth);
		CurrentVisuals.Contrast = FMath::Lerp(PreviousVisuals.Contrast, TargetVisuals.Contrast, Smooth);
		CurrentVisuals.Gamma = FMath::Lerp(PreviousVisuals.Gamma, TargetVisuals.Gamma, Smooth);
		CurrentVisuals.ShadowBrightness = FMath::Lerp(PreviousVisuals.ShadowBrightness, TargetVisuals.ShadowBrightness, Smooth);
		CurrentVisuals.HighlightBrightness = FMath::Lerp(PreviousVisuals.HighlightBrightness, TargetVisuals.HighlightBrightness, Smooth);
	}
}

void UMistspireVisualEnhancementSubsystem::ApplyCurrentVisuals()
{
	float Intensity = OverrideIntensity;
	ApplyBloom(CurrentVisuals.BloomIntensity * Intensity, CurrentVisuals.BloomThreshold);
	ApplyVolumetricFog(CurrentVisuals.VolumetricFogDensity * Intensity, CurrentVisuals.VolumetricFogAlbedo, CurrentVisuals.VolumetricFogColor);
	ApplyColorGrading(CurrentVisuals.ColorTemperature, CurrentVisuals.ColorTintShadows, CurrentVisuals.Saturation, CurrentVisuals.Contrast, CurrentVisuals.Gamma);
	ApplyAO(CurrentVisuals.AmbientOcclusionIntensity * Intensity);
}

void UMistspireVisualEnhancementSubsystem::ForceBiomeVisuals(EMistspireBiomeType Biome)
{
	if (Biome != CurrentBiome)
	{
		PreviousVisuals = CurrentVisuals;
		CurrentBiome = Biome;
		TargetVisuals = MakeBiomeVisuals(Biome);
		TransitionAlpha = 0.f;
		ElapsedTime = 0.f;
	}
	bForced = true;
	ForcedBiome = Biome;
}

void UMistspireVisualEnhancementSubsystem::ClearForcedBiome()
{
	bForced = false;
}

void UMistspireVisualEnhancementSubsystem::ApplyBloom(float Intensity, float Threshold)
{
	static const auto CVarBloomIntensity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomIntensity"));
	static const auto CVarBloomThreshold = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomThreshold"));
	if (CVarBloomIntensity) CVarBloomIntensity->Set(Intensity, ECVF_SetByScalability);
	if (CVarBloomThreshold) CVarBloomThreshold->Set(Threshold, ECVF_SetByScalability);
}

void UMistspireVisualEnhancementSubsystem::ApplyVolumetricFog(float Density, float Albedo, const FLinearColor& Color)
{
	static const auto CVarFogDensity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog.Density"));
	static const auto CVarFogAlbedo = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog.Albedo"));
	static const auto CVarFogColorR = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog.Color.R"));
	static const auto CVarFogColorG = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog.Color.G"));
	static const auto CVarFogColorB = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog.Color.B"));
	if (CVarFogDensity) CVarFogDensity->Set(Density, ECVF_SetByScalability);
	if (CVarFogAlbedo) CVarFogAlbedo->Set(Albedo, ECVF_SetByScalability);
	if (CVarFogColorR) CVarFogColorR->Set(Color.R, ECVF_SetByScalability);
	if (CVarFogColorG) CVarFogColorG->Set(Color.G, ECVF_SetByScalability);
	if (CVarFogColorB) CVarFogColorB->Set(Color.B, ECVF_SetByScalability);
}

void UMistspireVisualEnhancementSubsystem::ApplyColorGrading(float Temperature, float Tint, float Saturation, float Contrast, float Gamma)
{
	static const auto CVarColorTemp = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ColorTemperature"));
	static const auto CVarColorTint = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ColorTintShadows"));
	static const auto CVarSaturation = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Color.Saturation"));
	static const auto CVarContrast = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Color.Contrast"));
	static const auto CVarGamma = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Tonemapper.Gamma"));
	if (CVarColorTemp) CVarColorTemp->Set(Temperature, ECVF_SetByScalability);
	if (CVarColorTint) CVarColorTint->Set(Tint, ECVF_SetByScalability);
	if (CVarSaturation) CVarSaturation->Set(Saturation, ECVF_SetByScalability);
	if (CVarContrast) CVarContrast->Set(Contrast, ECVF_SetByScalability);
	if (CVarGamma) CVarGamma->Set(Gamma, ECVF_SetByScalability);
}

void UMistspireVisualEnhancementSubsystem::ApplyAO(float Intensity)
{
	static const auto CVarAOIntensity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientOcclusion.DistanceScale"));
	if (CVarAOIntensity) CVarAOIntensity->Set(Intensity, ECVF_SetByScalability);
}

FMistspireBiomeVisuals UMistspireVisualEnhancementSubsystem::MakeBiomeVisuals(EMistspireBiomeType Biome)
{
	FMistspireBiomeVisuals V;

	switch (Biome)
	{
	case EMistspireBiomeType::Mist:
		V.BloomIntensity = 0.6f;
		V.BloomThreshold = 2.0f;
		V.AmbientOcclusionIntensity = 0.5f;
		V.VolumetricFogDensity = 0.25f;
		V.VolumetricFogAlbedo = 0.85f;
		V.VolumetricFogColor = FLinearColor(0.7f, 0.75f, 0.8f);
		V.ColorTemperature = 5500.f;
		V.Saturation = 0.7f;
		V.Contrast = 0.9f;
		V.Gamma = 2.2f;
		V.ShadowBrightness = 0.5f;
		V.HighlightBrightness = 0.9f;
		break;

	case EMistspireBiomeType::Arid:
		V.BloomIntensity = 0.8f;
		V.BloomThreshold = 1.8f;
		V.AmbientOcclusionIntensity = 0.7f;
		V.VolumetricFogDensity = 0.08f;
		V.VolumetricFogAlbedo = 0.6f;
		V.VolumetricFogColor = FLinearColor(0.9f, 0.7f, 0.4f);
		V.ColorTemperature = 7500.f;
		V.Saturation = 0.9f;
		V.Contrast = 1.1f;
		V.Gamma = 2.1f;
		V.ShadowBrightness = 0.35f;
		V.HighlightBrightness = 1.1f;
		break;

	case EMistspireBiomeType::Forest:
		V.BloomIntensity = 1.2f;
		V.BloomThreshold = 1.2f;
		V.AmbientOcclusionIntensity = 0.8f;
		V.VolumetricFogDensity = 0.12f;
		V.VolumetricFogAlbedo = 0.7f;
		V.VolumetricFogColor = FLinearColor(0.2f, 0.5f, 0.2f);
		V.ColorTemperature = 5800.f;
		V.Saturation = 1.1f;
		V.Contrast = 1.0f;
		V.Gamma = 2.2f;
		V.ShadowBrightness = 0.4f;
		V.HighlightBrightness = 1.0f;
		break;

	case EMistspireBiomeType::Ember:
		V.BloomIntensity = 2.0f;
		V.BloomThreshold = 0.8f;
		V.AmbientOcclusionIntensity = 0.9f;
		V.VolumetricFogDensity = 0.18f;
		V.VolumetricFogAlbedo = 0.65f;
		V.VolumetricFogColor = FLinearColor(0.8f, 0.25f, 0.05f);
		V.ColorTemperature = 9000.f;
		V.Saturation = 1.3f;
		V.Contrast = 1.2f;
		V.Gamma = 2.0f;
		V.ShadowBrightness = 0.2f;
		V.HighlightBrightness = 1.3f;
		break;

	case EMistspireBiomeType::Crystal:
		V.BloomIntensity = 1.5f;
		V.BloomThreshold = 1.0f;
		V.AmbientOcclusionIntensity = 0.6f;
		V.VolumetricFogDensity = 0.05f;
		V.VolumetricFogAlbedo = 0.9f;
		V.VolumetricFogColor = FLinearColor(0.6f, 0.8f, 1.0f);
		V.ColorTemperature = 4500.f;
		V.Saturation = 0.8f;
		V.Contrast = 1.3f;
		V.Gamma = 2.4f;
		V.ShadowBrightness = 0.5f;
		V.HighlightBrightness = 1.2f;
		break;

	case EMistspireBiomeType::Void:
		V.BloomIntensity = 0.3f;
		V.BloomThreshold = 3.0f;
		V.AmbientOcclusionIntensity = 1.0f;
		V.VolumetricFogDensity = 0.3f;
		V.VolumetricFogAlbedo = 0.4f;
		V.VolumetricFogColor = FLinearColor(0.1f, 0.05f, 0.2f);
		V.ColorTemperature = 3000.f;
		V.Saturation = 0.4f;
		V.Contrast = 1.4f;
		V.Gamma = 2.0f;
		V.ShadowBrightness = 0.2f;
		V.HighlightBrightness = 0.6f;
		break;

	case EMistspireBiomeType::Tundra:
		V.BloomIntensity = 1.0f;
		V.BloomThreshold = 1.5f;
		V.AmbientOcclusionIntensity = 0.5f;
		V.VolumetricFogDensity = 0.2f;
		V.VolumetricFogAlbedo = 0.95f;
		V.VolumetricFogColor = FLinearColor(0.8f, 0.85f, 0.9f);
		V.ColorTemperature = 5000.f;
		V.Saturation = 0.6f;
		V.Contrast = 1.0f;
		V.Gamma = 2.3f;
		V.ShadowBrightness = 0.5f;
		V.HighlightBrightness = 1.0f;
		break;

	case EMistspireBiomeType::Aether:
		V.BloomIntensity = 2.5f;
		V.BloomThreshold = 0.6f;
		V.AmbientOcclusionIntensity = 0.3f;
		V.VolumetricFogDensity = 0.04f;
		V.VolumetricFogAlbedo = 0.98f;
		V.VolumetricFogColor = FLinearColor(0.7f, 0.9f, 1.0f);
		V.ColorTemperature = 4000.f;
		V.Saturation = 0.9f;
		V.Contrast = 0.9f;
		V.Gamma = 2.4f;
		V.ShadowBrightness = 0.6f;
		V.HighlightBrightness = 1.4f;
		break;

	case EMistspireBiomeType::Sanctum:
		V.BloomIntensity = 3.0f;
		V.BloomThreshold = 0.4f;
		V.AmbientOcclusionIntensity = 0.4f;
		V.VolumetricFogDensity = 0.02f;
		V.VolumetricFogAlbedo = 0.99f;
		V.VolumetricFogColor = FLinearColor(1.0f, 0.95f, 0.8f);
		V.ColorTemperature = 3500.f;
		V.Saturation = 0.7f;
		V.Contrast = 0.85f;
		V.Gamma = 2.5f;
		V.ShadowBrightness = 0.7f;
		V.HighlightBrightness = 1.5f;
		break;

	case EMistspireBiomeType::Pinnacle:
		V.BloomIntensity = 1.8f;
		V.BloomThreshold = 0.9f;
		V.AmbientOcclusionIntensity = 0.2f;
		V.VolumetricFogDensity = 0.01f;
		V.VolumetricFogAlbedo = 0.5f;
		V.VolumetricFogColor = FLinearColor(0.05f, 0.05f, 0.15f);
		V.ColorTemperature = 2500.f;
		V.Saturation = 0.5f;
		V.Contrast = 1.5f;
		V.Gamma = 2.0f;
		V.ShadowBrightness = 0.3f;
		V.HighlightBrightness = 1.2f;
		break;

	default: break;
	}
	return V;
}
