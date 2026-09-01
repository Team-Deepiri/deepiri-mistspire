#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireBeaconSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireInputMode.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<int32> CVarMistspireShowAltitudeHud(
	TEXT("mistspire.ShowAltitudeHUD"),
	1,
	TEXT("Draw immersive survival HUD on screen (0=off, 1=on)."),
	ECVF_Default);

static TAutoConsoleVariable<int32> CVarMistspireShowControls(
	TEXT("mistspire.ShowControls"),
	1,
	TEXT("Draw non-VR control hints on screen (0=off, 1=on)."),
	ECVF_Default);

static TAutoConsoleVariable<int32> CVarMistspireAltitudeLogInterval(
	TEXT("mistspire.AltitudeLogInterval"),
	0,
	TEXT("Seconds between altitude log lines (0=disabled)."),
	ECVF_Default);

bool UMistspireAltitudeDebugSubsystem::IsHudEnabled()
{
	return CVarMistspireShowAltitudeHud.GetValueOnGameThread() != 0;
}

void UMistspireAltitudeDebugSubsystem::SetHudEnabled(bool bEnabled)
{
	CVarMistspireShowAltitudeHud->Set(bEnabled ? 1 : 0);
}

bool UMistspireAltitudeDebugSubsystem::IsTickable() const
{
	return !IsTemplate();
}

TStatId UMistspireAltitudeDebugSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireAltitudeDebugSubsystem, STATGROUP_Tickables);
}

void UMistspireAltitudeDebugSubsystem::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UMistspireAltitudeSubsystem* Alt = World->GetSubsystem<UMistspireAltitudeSubsystem>();
	if (!Alt)
	{
		return;
	}

	const float CurrentM = Alt->GetCurrentAltitudeCm() / 100.f;
	const float BestM = Alt->GetPersonalBestAltitudeCm() / 100.f;
	const bool bNonVR = FMistspireInputMode::IsNonVRMode(World);

	if (IsHudEnabled() && GEngine)
	{
		FString Line1 = FString::Printf(TEXT("Mistspire  %.0f m   BEST %.0f m"), CurrentM, BestM);

		if (UMistspireZoneSubsystem* Zone = World->GetSubsystem<UMistspireZoneSubsystem>())
		{
			Line1 += FString::Printf(TEXT("   [%s]"), *UMistspireZoneSubsystem::GetZoneDisplayName(Zone->GetCurrentZone()).ToString());
		}

		if (UMistspireWorldAtlasSubsystem* Atlas = World->GetSubsystem<UMistspireWorldAtlasSubsystem>())
		{
			Line1 += FString::Printf(TEXT("   { %s }"),
				*UMistspireWorldAtlasSubsystem::GetDistrictDisplayName(Atlas->GetCurrentDistrict()).ToString());
		}

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Cyan, Line1);

		if (UMistspireInteriorSubsystem* Interior = World->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			if (Interior->IsInsideInterior())
			{
				GEngine->AddOnScreenDebugMessage(
					3, 0.f, FColor::Yellow,
					FString::Printf(TEXT("INTERIOR: %s"), *Interior->GetCurrentBuildingId().ToString()));
			}
		}

		if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			FString WeatherLine = FString::Printf(TEXT("Weather: %s"), *Env->GetWeatherDisplayName().ToString());

			const EMistspireBiomeType Biome = Env->GetCurrentBiome();
			switch (Biome)
			{
				case EMistspireBiomeType::Mist: WeatherLine += TEXT("   Biome: MIST"); break;
				case EMistspireBiomeType::Arid: WeatherLine += TEXT("   Biome: ARID"); break;
				case EMistspireBiomeType::Forest: WeatherLine += TEXT("   Biome: FOREST"); break;
				case EMistspireBiomeType::Ember: WeatherLine += TEXT("   Biome: EMBER"); break;
				case EMistspireBiomeType::Crystal: WeatherLine += TEXT("   Biome: CRYSTAL"); break;
				case EMistspireBiomeType::Void: WeatherLine += TEXT("   Biome: VOID"); break;
				case EMistspireBiomeType::Tundra: WeatherLine += TEXT("   Biome: TUNDRA"); break;
				case EMistspireBiomeType::Aether: WeatherLine += TEXT("   Biome: AETHER"); break;
				case EMistspireBiomeType::Sanctum: WeatherLine += TEXT("   Biome: SANCTUM"); break;
				case EMistspireBiomeType::Pinnacle: WeatherLine += TEXT("   Biome: PINNACLE"); break;
				default: break;
			}

			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Silver, WeatherLine);
		}

		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(PC->GetPawn()))
			{
				GEngine->AddOnScreenDebugMessage(
					2, 0.f, FColor::Green,
					FString::Printf(TEXT("STA %.0f%%  O2 %.0f%%  P %.2f atm"),
						100.f * Pawn->GetStaminaPercent(),
						100.f * Pawn->GetOxygenPercent(),
						Pawn->GetAtmosphericPressure()));

				if (UMistspireBeaconSubsystem* Beacon = World->GetSubsystem<UMistspireBeaconSubsystem>())
				{
					const FMistspireBeaconTarget Target = Beacon->GetCachedBeacon();
					if (Target.bValid)
					{
						GEngine->AddOnScreenDebugMessage(
							4, 0.f, FColor::Orange,
							FString::Printf(TEXT("Beacon: %.1f km  bearing %.0f deg"),
								Target.DistanceCm / 100000.f, Target.BearingDegrees));
					}
				}
			}
		}
	}

	if (bNonVR && GEngine && CVarMistspireShowControls.GetValueOnGameThread() != 0)
	{
		GEngine->AddOnScreenDebugMessage(
			5, 0.f, FColor::White,
			FMistspireInputMode::GetNonVRControlsHint());
	}

	const float Interval = static_cast<float>(CVarMistspireAltitudeLogInterval.GetValueOnGameThread());
	if (Interval > 0.f)
	{
		LogAccumulator += DeltaTime;
		if (LogAccumulator >= Interval)
		{
			LogAccumulator = 0.f;
			UE_LOG(LogTemp, Log, TEXT("Mistspire altitude: current=%.0fcm personal_best=%.0fcm"),
				Alt->GetCurrentAltitudeCm(), Alt->GetPersonalBestAltitudeCm());
		}
	}
}

void UMistspireAltitudeDebugSubsystem::LogAltitudeStats()
{
	if (UMistspireAltitudeSubsystem* Alt = GetWorld() ? GetWorld()->GetSubsystem<UMistspireAltitudeSubsystem>() : nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Mistspire: current=%.0f cm, personal_best=%.0f cm"),
			Alt->GetCurrentAltitudeCm(), Alt->GetPersonalBestAltitudeCm());
	}
}
