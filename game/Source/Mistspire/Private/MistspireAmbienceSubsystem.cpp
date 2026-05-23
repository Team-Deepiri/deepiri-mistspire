#include "MistspireAmbienceSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireZoneSubsystem.h"

TStatId UMistspireAmbienceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireAmbienceSubsystem, STATGROUP_Tickables);
}

void UMistspireAmbienceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float ZoneFactor = 0.3f;
	if (UMistspireZoneSubsystem* Zone = GetWorld()->GetSubsystem<UMistspireZoneSubsystem>())
	{
		DominantZone = Zone->GetCurrentZone();
		ZoneFactor = Zone->GetZoneAmbientIntensity();
	}

	float WeatherTension = 0.f;
	float WeatherMystery = 0.f;
	if (UMistspireEnvironmentSubsystem* Env = GetWorld()->GetSubsystem<UMistspireEnvironmentSubsystem>())
	{
		switch (Env->GetCurrentWeather())
		{
			case EMistspireWeatherType::MistStorm:
				WeatherTension = 0.5f;
				WeatherMystery = 0.7f;
				break;
			case EMistspireWeatherType::ElectricTurmoil:
				WeatherTension = 0.9f;
				WeatherMystery = 0.4f;
				break;
			case EMistspireWeatherType::ZenithGlow:
				WeatherTension = 0.1f;
				WeatherMystery = 0.95f;
				break;
			default:
				break;
		}
	}

	float AltFactor = 0.f;
	if (UMistspireAltitudeSubsystem* Alt = GetWorld()->GetSubsystem<UMistspireAltitudeSubsystem>())
	{
		AltFactor = FMath::Clamp(Alt->GetCurrentAltitudeCm() / 800000.f, 0.f, 1.f);
	}

	const float TargetTension = FMath::Clamp(ZoneFactor * 0.4f + WeatherTension + AltFactor * 0.3f, 0.f, 1.f);
	const float TargetMystery = FMath::Clamp(ZoneFactor * 0.3f + WeatherMystery + AltFactor * 0.2f, 0.f, 1.f);

	TensionLevel = FMath::FInterpTo(TensionLevel, TargetTension, DeltaTime, 1.5f);
	MysteryLevel = FMath::FInterpTo(MysteryLevel, TargetMystery, DeltaTime, 1.5f);
}
