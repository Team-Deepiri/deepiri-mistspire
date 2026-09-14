#include "MistspireEnvironmentSubsystem.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireDemoSpireLayout.h"
#include "MistspireGameState.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInterface.h"

void UMistspireEnvironmentSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeAccumulator += DeltaTime;
	UpdateWeather(DeltaTime);
	UpdateWeatherPresentation(DeltaTime);
	UpdateSkydomeCoverage();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			CurrentBiome = BiomeFromAltitude(Pawn->GetActorLocation().Z);
		}
	}
}

TStatId UMistspireEnvironmentSubsystem::GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireEnvironmentSubsystem, STATGROUP_Tickables); }

namespace
{
	bool IsSkydomeActor(const AActor* Actor)
	{
		if (!Actor)
		{
			return false;
		}

		const FString ClassName = Actor->GetClass()->GetName();
		if (ClassName.Contains(TEXT("Sky_Sphere")) || ClassName.Contains(TEXT("SkySphere")))
		{
			return true;
		}

		TArray<UStaticMeshComponent*> MeshComps;
		Actor->GetComponents<UStaticMeshComponent>(MeshComps);
		for (const UStaticMeshComponent* Comp : MeshComps)
		{
			if (!Comp)
			{
				continue;
			}
			for (int32 MatIdx = 0; MatIdx < Comp->GetNumMaterials(); ++MatIdx)
			{
				if (const UMaterialInterface* Mat = Comp->GetMaterial(MatIdx))
				{
					const FString MatPath = Mat->GetPathName();
					if (MatPath.Contains(TEXT("/Engine/EngineSky/"), ESearchCase::IgnoreCase)
						|| MatPath.Contains(TEXT("Sky_Material"), ESearchCase::IgnoreCase))
					{
						return true;
					}
				}
			}
		}
		return false;
	}
}

void UMistspireEnvironmentSubsystem::ResolveSkydomeActors()
{
	bSkydomeActorsResolved = true;
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	using namespace MistspireDemoSpire;
	const float MaxCameraZ = GetValleyOrigin().Z + StationAltitudeCm[9] + TourLandingClearanceCm + 500000.f;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsActorBeingDestroyed() || !IsSkydomeActor(Actor))
		{
			continue;
		}

		SkyDomeActors.Add(Actor);

		if (const UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>())
		{
			if (const UStaticMesh* Mesh = MeshComp->GetStaticMesh())
			{
				const float MeshRadius = static_cast<float>(Mesh->GetBounds().SphereRadius);
				if (MeshRadius > 1.f)
				{
					// Template OpenWorld domes sit at origin; DemoTour Pinnacle is ~19 km up.
					const float NeededScale = (MaxCameraZ * 1.35f) / MeshRadius;
					SkydomeMinUniformScale = FMath::Max(SkydomeMinUniformScale, NeededScale);
				}
			}
		}
	}

	if (SkyDomeActors.Num() > 0)
	{
		UE_LOG(LogTemp, Log,
			TEXT("Mistspire Environment: tracking %d skydome actor(s), minUniformScale=%.0f."),
			SkyDomeActors.Num(), SkydomeMinUniformScale);
	}
}

void UMistspireEnvironmentSubsystem::UpdateSkydomeCoverage()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!bSkydomeActorsResolved)
	{
		ResolveSkydomeActors();
	}

	if (SkyDomeActors.Num() == 0)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	FVector ViewLoc = FVector::ZeroVector;
	FRotator ViewRot = FRotator::ZeroRotator;
	PC->GetPlayerViewPoint(ViewLoc, ViewRot);

	for (TWeakObjectPtr<AActor>& WeakSky : SkyDomeActors)
	{
		AActor* Sky = WeakSky.Get();
		if (!Sky || Sky->IsActorBeingDestroyed())
		{
			continue;
		}

		// Keep the camera inside the inverted sky mesh so the EngineSky material covers the frustum.
		Sky->SetActorLocation(ViewLoc, false, nullptr, ETeleportType::None);

		if (SkydomeMinUniformScale > 0.f)
		{
			const FVector Scale = Sky->GetActorScale3D();
			const float CurrentUniform = FMath::Max3(Scale.X, Scale.Y, Scale.Z);
			if (CurrentUniform < SkydomeMinUniformScale)
			{
				Sky->SetActorScale3D(FVector(SkydomeMinUniformScale));
			}
		}
	}
}

void UMistspireEnvironmentSubsystem::UpdateWeather(float DeltaTime)
{
	if (ForcedWeatherHold > 0.f)
	{
		ForcedWeatherHold -= DeltaTime;
		return;
	}

	if (!GetWorld()->IsNetMode(NM_Client))
	{
		WeatherTransitionTimer -= DeltaTime;
		if (WeatherTransitionTimer <= 0.f)
		{
			WeatherTransitionTimer = FMath::FRandRange(30.f, 120.f);
			
			float Roll = FMath::FRand();
			if (Roll < 0.6f) CurrentWeather = EMistspireWeatherType::Clear;
			else if (Roll < 0.8f) CurrentWeather = EMistspireWeatherType::MistStorm;
			else if (Roll < 0.95f) CurrentWeather = EMistspireWeatherType::ElectricTurmoil;
			else CurrentWeather = EMistspireWeatherType::ZenithGlow;

			// Sync to GameState
			if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
			{
				GS->CurrentWeatherIndex = (uint8)CurrentWeather;
			}
		}
	}
	else
	{
		// Clients follow the GameState
		if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
		{
			CurrentWeather = (EMistspireWeatherType)GS->CurrentWeatherIndex;
		}
	}
}

FVector UMistspireEnvironmentSubsystem::GetWindAtAltitude(float AltitudeCm) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			if (Interior->IsInsideInterior())
			{
				return FVector::ZeroVector;
			}
		}
	}

	// Wind strength increases with altitude
	float BaseStrength = (AltitudeCm / 100000.f) * 300.f; 
	
	// Weather Multiplier
	float WeatherMult = 1.0f;
	switch (CurrentWeather)
	{
		case EMistspireWeatherType::MistStorm: WeatherMult = 2.5f; break;
		case EMistspireWeatherType::ElectricTurmoil: WeatherMult = 1.8f; break;
		case EMistspireWeatherType::ZenithGlow: WeatherMult = 0.5f; break;
		default: break;
	}

	// Vary direction over time and altitude for turbulence
	float DirTime = GetWorld()->GetTimeSeconds();
	float Angle = (DirTime * 0.2f) + (AltitudeCm * 0.00005f);
	
	FVector WindDir(FMath::Cos(Angle), FMath::Sin(Angle), 0.1f * FMath::Sin(DirTime * 0.5f));
	FVector BaseWind = WindDir * BaseStrength * WeatherMult;

	// Dynamic Gusts (scale with weather)
	float GustFrequency = (0.1f + (AltitudeCm * 0.000001f)) * WeatherMult;
	float GustSeed = DirTime * GustFrequency;
	float GustStrength = FMath::PerlinNoise1D(GustSeed);
	
	if (GustStrength > 0.6f)
	{
		float ExtraPush = (GustStrength - 0.6f) * 1500.f * WeatherMult;
		BaseWind += WindDir * ExtraPush;
	}

	return BaseWind;
}

float UMistspireEnvironmentSubsystem::GetMistDensityAtAltitude(float AltitudeCm) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			if (Interior->IsInsideInterior())
			{
				return 0.08f;
			}
		}
	}

	float Density = FMath::Clamp(1.0f - (AltitudeCm / 150000.f), 0.05f, 1.0f);
	switch (CurrentWeather)
	{
		case EMistspireWeatherType::MistStorm: Density = FMath::Min(1.f, Density * 1.6f); break;
		case EMistspireWeatherType::ZenithGlow: Density *= 0.4f; break;
		default: break;
	}
	return Density;
}

float UMistspireEnvironmentSubsystem::GetDayNightBlend() const
{
	return 0.5f + 0.5f * FMath::Sin(TimeAccumulator * 0.02f);
}

float UMistspireEnvironmentSubsystem::GetAuroraIntensity(float AltitudeCm) const
{
	if (CurrentWeather != EMistspireWeatherType::ZenithGlow && CurrentWeather != EMistspireWeatherType::Clear)
	{
		return 0.f;
	}
	const float AltFactor = FMath::Clamp((AltitudeCm - 400000.f) / 400000.f, 0.f, 1.f);
	return AltFactor * (1.f - GetDayNightBlend()) * 0.85f;
}

void UMistspireEnvironmentSubsystem::ForceWeather(EMistspireWeatherType Weather, float HoldSeconds)
{
	CurrentWeather = Weather;
	ForcedWeatherHold = HoldSeconds;
	WeatherTransitionTimer = HoldSeconds;
	if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
	{
		GS->CurrentWeatherIndex = static_cast<uint8>(Weather);
	}
	// Kick presentation immediately so the porch button / SetWeather reads on the next frame.
	PresentedWeather = Weather;
}

UMistspireEnvironmentSubsystem::FWeatherSkyLook UMistspireEnvironmentSubsystem::MakeWeatherSkyLook(
	EMistspireWeatherType Weather)
{
	FWeatherSkyLook Look;
	switch (Weather)
	{
	case EMistspireWeatherType::MistStorm:
		Look.SkyTint = FLinearColor(0.72f, 0.76f, 0.82f);
		Look.Rayleigh = FLinearColor(0.45f, 0.50f, 0.58f);
		Look.FogColor = FLinearColor(0.78f, 0.82f, 0.88f);
		Look.FogDensity = 0.085f;
		Look.SunColor = FLinearColor(0.75f, 0.78f, 0.85f);
		Look.SunIntensityScale = 0.45f;
		break;
	case EMistspireWeatherType::ElectricTurmoil:
		Look.SkyTint = FLinearColor(0.35f, 0.28f, 0.75f);
		Look.Rayleigh = FLinearColor(0.22f, 0.18f, 0.95f);
		Look.FogColor = FLinearColor(0.25f, 0.35f, 0.85f);
		Look.FogDensity = 0.045f;
		Look.SunColor = FLinearColor(0.55f, 0.70f, 1.0f);
		Look.SunIntensityScale = 0.7f;
		break;
	case EMistspireWeatherType::ZenithGlow:
		Look.SkyTint = FLinearColor(1.0f, 0.72f, 0.45f);
		Look.Rayleigh = FLinearColor(0.95f, 0.55f, 0.35f);
		Look.FogColor = FLinearColor(1.0f, 0.82f, 0.55f);
		Look.FogDensity = 0.018f;
		Look.SunColor = FLinearColor(1.0f, 0.85f, 0.55f);
		Look.SunIntensityScale = 1.35f;
		break;
	case EMistspireWeatherType::Clear:
	default:
		Look.SkyTint = FLinearColor(1.f, 1.f, 1.f);
		Look.Rayleigh = FLinearColor(0.175287f, 0.409607f, 1.f);
		Look.FogColor = FLinearColor(0.55f, 0.68f, 0.92f);
		Look.FogDensity = 0.02f;
		Look.SunColor = FLinearColor(1.f, 0.96f, 0.88f);
		Look.SunIntensityScale = 1.f;
		break;
	}
	return Look;
}

void UMistspireEnvironmentSubsystem::UpdateWeatherPresentation(float DeltaTime)
{
	const FWeatherSkyLook Target = MakeWeatherSkyLook(CurrentWeather);
	if (!bSkyLookInitialized)
	{
		AppliedSkyLook = Target;
		bSkyLookInitialized = true;
		PresentedWeather = CurrentWeather;
	}
	else
	{
		// Snappy enough for the porch button demo beat (~1.2 s to settle).
		const float Alpha = 1.f - FMath::Exp(-DeltaTime * 2.8f);
		AppliedSkyLook.SkyTint = FLinearColor::LerpUsingHSV(AppliedSkyLook.SkyTint, Target.SkyTint, Alpha);
		AppliedSkyLook.Rayleigh = FLinearColor::LerpUsingHSV(AppliedSkyLook.Rayleigh, Target.Rayleigh, Alpha);
		AppliedSkyLook.FogColor = FLinearColor::LerpUsingHSV(AppliedSkyLook.FogColor, Target.FogColor, Alpha);
		AppliedSkyLook.FogDensity = FMath::Lerp(AppliedSkyLook.FogDensity, Target.FogDensity, Alpha);
		AppliedSkyLook.SunColor = FLinearColor::LerpUsingHSV(AppliedSkyLook.SunColor, Target.SunColor, Alpha);
		AppliedSkyLook.SunIntensityScale = FMath::Lerp(AppliedSkyLook.SunIntensityScale, Target.SunIntensityScale, Alpha);
	}

	ApplyWeatherToSkyActors(
		AppliedSkyLook.SkyTint,
		AppliedSkyLook.Rayleigh,
		AppliedSkyLook.FogColor,
		AppliedSkyLook.FogDensity,
		AppliedSkyLook.SunColor,
		AppliedSkyLook.SunIntensityScale);
}

void UMistspireEnvironmentSubsystem::ApplyWeatherToSkyActors(
	const FLinearColor& SkyTint,
	const FLinearColor& Rayleigh,
	const FLinearColor& FogColor,
	float FogDensity,
	const FLinearColor& SunColor,
	float SunIntensityScale)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ASkyAtmosphere> It(World); It; ++It)
	{
		if (USkyAtmosphereComponent* Sky = It->GetComponent())
		{
			Sky->SetSkyLuminanceFactor(SkyTint);
			Sky->SetRayleighScattering(Rayleigh);
		}
	}

	for (TActorIterator<AExponentialHeightFog> It(World); It; ++It)
	{
		if (UExponentialHeightFogComponent* Fog = It->GetComponent())
		{
			Fog->SetFogDensity(FogDensity);
			Fog->SetFogInscatteringColor(FogColor);
			Fog->SetDirectionalInscatteringColor(FogColor * 1.15f);
		}
	}

	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		ADirectionalLight* Sun = *It;
		if (!Sun || Sun->IsActorBeingDestroyed())
		{
			continue;
		}
		if (UDirectionalLightComponent* SunComp = Cast<UDirectionalLightComponent>(Sun->GetLightComponent()))
		{
			if (!bCachedSunIntensity)
			{
				CachedSunIntensity = SunComp->Intensity;
				bCachedSunIntensity = true;
			}
			SunComp->SetLightColor(SunColor);
			SunComp->SetIntensity(CachedSunIntensity * SunIntensityScale);
		}
	}
}

FText UMistspireEnvironmentSubsystem::GetWeatherDisplayName() const
{
	switch (CurrentWeather)
	{
		case EMistspireWeatherType::MistStorm: return NSLOCTEXT("Mistspire", "WeatherMist", "Mist Storm");
		case EMistspireWeatherType::ElectricTurmoil: return NSLOCTEXT("Mistspire", "WeatherElectric", "Electric Turmoil");
		case EMistspireWeatherType::ZenithGlow: return NSLOCTEXT("Mistspire", "WeatherZenith", "Zenith Glow");
		default: return NSLOCTEXT("Mistspire", "WeatherClear", "Clear");
	}
}

float UMistspireEnvironmentSubsystem::GetAtmosphericPressure(float AltitudeCm) const
{
	// Logarithmic pressure drop (Standard Atmosphere approx)
	// Base is 1.0 (Sea Level), dropping to ~0.2 at 12km (Zenith)
	float Pressure = FMath::Exp(-AltitudeCm / 800000.f); 
	return FMath::Clamp(Pressure, 0.05f, 1.0f);
}

float UMistspireEnvironmentSubsystem::GetTemperatureCelsius(float AltitudeCm) const
{
	// Lapse rate: -6.5C per km
	float BaseTemp = 25.0f; // Valley is warm
	float LapseRate = 0.0065f; // per cm
	float Temp = BaseTemp - (AltitudeCm * LapseRate);
	return Temp;
}

EMistspireBiomeType UMistspireEnvironmentSubsystem::BiomeFromAltitude(float AltitudeCm)
{
	if (AltitudeCm < 100000.f) return EMistspireBiomeType::Mist;
	if (AltitudeCm < 300000.f) return EMistspireBiomeType::Arid;
	if (AltitudeCm < 500000.f) return EMistspireBiomeType::Forest;
	if (AltitudeCm < 700000.f) return EMistspireBiomeType::Ember;
	if (AltitudeCm < 900000.f) return EMistspireBiomeType::Crystal;
	if (AltitudeCm < 1200000.f) return EMistspireBiomeType::Void;
	if (AltitudeCm < 1400000.f) return EMistspireBiomeType::Tundra;
	if (AltitudeCm < 1600000.f) return EMistspireBiomeType::Aether;
	if (AltitudeCm < 1800000.f) return EMistspireBiomeType::Sanctum;
	if (AltitudeCm < 2000000.f) return EMistspireBiomeType::Pinnacle;
	return EMistspireBiomeType::Pinnacle;
}
