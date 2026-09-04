#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireGameUserSettings.h"
#include "MistspireInputMode.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"

static TAutoConsoleVariable<int32> CVarMistspireShowAltitudeHud(
	TEXT("mistspire.ShowAltitudeHUD"),
	1,
	TEXT("Draw immersive survival HUD on screen (0=off, 1=on)."),
	ECVF_Default);

static TAutoConsoleVariable<int32> CVarMistspireShowControls(
	TEXT("mistspire.ShowControls"),
	-1,
	TEXT("Override control-hint HUD: -1=use settings (default), 0=force off, 1=force on."),
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

bool UMistspireAltitudeDebugSubsystem::IsControlsHintEnabled()
{
	const int32 Override = CVarMistspireShowControls.GetValueOnGameThread();
	if (Override == 0)
	{
		return false;
	}
	if (Override > 0)
	{
		return true;
	}
	if (const UMistspireGameUserSettings* Settings = UMistspireGameUserSettings::Get())
	{
		return Settings->IsControlsHintEnabled();
	}
	return false;
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
	// Visual HUD is drawn by AMistspireHUD (Canvas) — not AddOnScreenDebugMessage —
	// so Engine never shows "'DisableAllScreenMessages' to suppress".
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
