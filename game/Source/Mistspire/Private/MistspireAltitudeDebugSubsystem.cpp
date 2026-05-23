#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "MistspireVRPawn.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<int32> CVarMistspireShowAltitudeHud(
	TEXT("mistspire.ShowAltitudeHUD"),
	1,
	TEXT("Draw immersive survival HUD on screen (0=off, 1=on)."),
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

	if (IsHudEnabled() && GEngine)
	{
		FString Line1 = FString::Printf(TEXT("Mistspire  %.0f m   BEST %.0f m"), CurrentM, BestM);

		if (UMistspireZoneSubsystem* Zone = World->GetSubsystem<UMistspireZoneSubsystem>())
		{
			Line1 += FString::Printf(TEXT("   [%s]"), *UMistspireZoneSubsystem::GetZoneDisplayName(Zone->GetCurrentZone()).ToString());
		}

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Cyan, Line1);

		if (UMistspireEnvironmentSubsystem* Env = World->GetSubsystem<UMistspireEnvironmentSubsystem>())
		{
			GEngine->AddOnScreenDebugMessage(
				1, 0.f, FColor::Silver,
				FString::Printf(TEXT("Weather: %s"), *Env->GetWeatherDisplayName().ToString()));
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
			}
		}
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
