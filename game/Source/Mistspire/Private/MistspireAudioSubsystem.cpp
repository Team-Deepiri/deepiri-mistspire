#include "MistspireAudioSubsystem.h"
#include "MistspireLog.h"
#include "MistspireEnvironmentSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDevice.h"

void UMistspireAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (int32 i = 0; i < 6; ++i)
	{
		BusStates[i].VolumeMultiplier = 1.0f;
		BusStates[i].PitchMultiplier = 1.0f;
		BusStates[i].bMuted = false;
		BusStates[i].FilterCutoffHz = 20000.0f;
	}

	FAudioDevice* AudioDevice = GetWorld()->GetAudioDeviceRaw();
	if (AudioDevice)
	{
		UE_LOG(LogMistspire, Log, TEXT("MistspireAudioSubsystem initialized with 6 audio channels"));
	}
}

FMistspireAudioBusState& UMistspireAudioSubsystem::GetBus(EMistspireAudioChannel Channel)
{
	return BusStates[static_cast<int32>(Channel)];
}

void UMistspireAudioSubsystem::SetChannelVolume(EMistspireAudioChannel Channel, float Volume)
{
	FMistspireAudioBusState& Bus = GetBus(Channel);
	Bus.VolumeMultiplier = FMath::Clamp(Volume, 0.0f, 2.0f);
}

void UMistspireAudioSubsystem::SetChannelPitch(EMistspireAudioChannel Channel, float Pitch)
{
	FMistspireAudioBusState& Bus = GetBus(Channel);
	Bus.PitchMultiplier = FMath::Clamp(Pitch, 0.25f, 4.0f);
}

void UMistspireAudioSubsystem::MuteChannel(EMistspireAudioChannel Channel, bool bMuted)
{
	GetBus(Channel).bMuted = bMuted;
}

float UMistspireAudioSubsystem::GetChannelVolume(EMistspireAudioChannel Channel) const
{
	return BusStates[static_cast<int32>(Channel)].VolumeMultiplier;
}

void UMistspireAudioSubsystem::PlayBiomeAmbience(FName BiomeName, float CrossfadeDuration)
{
	USoundCue** FoundCue = BiomeAmbienceMap.Find(BiomeName);
	if (!FoundCue || !*FoundCue)
	{
		UE_LOG(LogMistspire, Warning, TEXT("PlayBiomeAmbience: no sound cue for biome '%s'"), *BiomeName.ToString());
		return;
	}

	if (!AmbienceComponent)
	{
		AmbienceComponent = NewObject<UAudioComponent>(this);
		AmbienceComponent->bAutoDestroy = false;
		AmbienceComponent->bAllowAnyoneToDestroyMe = true;
		AmbienceComponent->SetupAttachment(GetWorld()->GetFirstPlayerController()->GetRootComponent());
		AmbienceComponent->RegisterComponent();
	}

	const float BusVolume = GetBus(EMistspireAudioChannel::Ambient).bMuted ? 0.f : BusStates[static_cast<int32>(EMistspireAudioChannel::Ambient)].VolumeMultiplier;
	AmbienceComponent->SetSound(*FoundCue);
	AmbienceComponent->SetVolumeMultiplier(BusVolume);
	AmbienceComponent->Play();
}

void UMistspireAudioSubsystem::PlayPhysiologySound(EPhysiologySoundType Type, float Intensity)
{
	USoundCue** FoundCue = PhysiologySoundMap.Find(Type);
	if (!FoundCue || !*FoundCue)
	{
		UE_LOG(LogMistspire, Warning, TEXT("PlayPhysiologySound: no sound cue for type %d"), static_cast<int32>(Type));
		return;
	}

	APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!Camera) return;

	const float BusVolume = GetBus(EMistspireAudioChannel::Physiology).bMuted ? 0.f : BusStates[static_cast<int32>(EMistspireAudioChannel::Physiology)].VolumeMultiplier;

	UAudioComponent* AudioComp = NewObject<UAudioComponent>(this);
	AudioComp->bAutoDestroy = true;
	AudioComp->bAllowAnyoneToDestroyMe = true;
	AudioComp->SetWorldLocation(Camera->GetCameraLocation());
	AudioComp->SetSound(*FoundCue);
	AudioComp->SetVolumeMultiplier(BusVolume * FMath::Clamp(Intensity, 0.0f, 2.0f));
	AudioComp->RegisterComponent();
	AudioComp->Play();
}

void UMistspireAudioSubsystem::PlayWeatherSound(EMistspireWeatherType Weather, float Intensity)
{
	USoundCue** FoundCue = WeatherSoundMap.Find(Weather);
	if (!FoundCue || !*FoundCue)
	{
		UE_LOG(LogMistspire, Warning, TEXT("PlayWeatherSound: no sound cue for weather %d"), static_cast<int32>(Weather));
		return;
	}

	if (!WeatherComponent)
	{
		WeatherComponent = NewObject<UAudioComponent>(this);
		WeatherComponent->bAutoDestroy = false;
		WeatherComponent->bAllowAnyoneToDestroyMe = true;
		WeatherComponent->SetupAttachment(GetWorld()->GetFirstPlayerController()->GetRootComponent());
		WeatherComponent->RegisterComponent();
	}

	const float BusVolume = GetBus(EMistspireAudioChannel::Weather).bMuted ? 0.f : BusStates[static_cast<int32>(EMistspireAudioChannel::Weather)].VolumeMultiplier;
	WeatherComponent->SetSound(*FoundCue);
	WeatherComponent->SetVolumeMultiplier(BusVolume * FMath::Clamp(Intensity, 0.0f, 1.5f));
	WeatherComponent->Play();
}

void UMistspireAudioSubsystem::PlaySurfaceContactSound(float ImpactForce, bool bIsStone)
{
	FName SurfaceType = bIsStone ? FName("Stone") : FName("Wood");
	USoundCue** FoundCue = BiomeAmbienceMap.Find(SurfaceType);
	if (!FoundCue || !*FoundCue)
	{
		UE_LOG(LogMistspire, Verbose, TEXT("PlaySurfaceContactSound: no cue for surface '%s'"), *SurfaceType.ToString());
		return;
	}

	const float NormalizedForce = FMath::Clamp(ImpactForce / 1000.0f, 0.0f, 1.0f);
	const float PitchVariation = FMath::FRandRange(0.85f, 1.15f);
	const float BusVolume = GetBus(EMistspireAudioChannel::Surface).bMuted ? 0.f : BusStates[static_cast<int32>(EMistspireAudioChannel::Surface)].VolumeMultiplier;

	UAudioComponent* AudioComp = NewObject<UAudioComponent>(this);
	AudioComp->bAutoDestroy = true;
	AudioComp->bAllowAnyoneToDestroyMe = true;
	AudioComp->SetSound(*FoundCue);
	AudioComp->SetVolumeMultiplier(BusVolume * NormalizedForce);
	AudioComp->SetPitchMultiplier(PitchVariation);
	AudioComp->RegisterComponent();
	AudioComp->Play();
}

void UMistspireAudioSubsystem::PlayUISound(FName SoundId, float Volume)
{
	USoundCue** FoundCue = UISoundMap.Find(SoundId);
	if (!FoundCue || !*FoundCue)
	{
		UE_LOG(LogMistspire, Verbose, TEXT("PlayUISound: no cue for '%s'"), *SoundId.ToString());
		return;
	}

	const float BusVolume = GetBus(EMistspireAudioChannel::UI).bMuted ? 0.f : BusStates[static_cast<int32>(EMistspireAudioChannel::UI)].VolumeMultiplier;

	UAudioComponent* AudioComp = NewObject<UAudioComponent>(this);
	AudioComp->bAutoDestroy = true;
	AudioComp->bAllowAnyoneToDestroyMe = true;
	AudioComp->SetSound(*FoundCue);
	AudioComp->SetVolumeMultiplier(BusVolume * FMath::Clamp(Volume, 0.0f, 2.0f));
	AudioComp->RegisterComponent();
	AudioComp->Play();
}

void UMistspireAudioSubsystem::PlaySpatialSoundAtLocation(FName SoundId, FVector WorldLocation, float Volume)
{
	USoundCue** FoundCue = SpatialSoundMap.Find(SoundId);
	if (!FoundCue || !*FoundCue)
	{
		UE_LOG(LogMistspire, Verbose, TEXT("PlaySpatialSoundAtLocation: no cue for '%s'"), *SoundId.ToString());
		return;
	}

	const float BusVolume = GetBus(EMistspireAudioChannel::Spatial).bMuted ? 0.f : BusStates[static_cast<int32>(EMistspireAudioChannel::Spatial)].VolumeMultiplier;

	UAudioComponent* AudioComp = NewObject<UAudioComponent>(this);
	AudioComp->bAutoDestroy = true;
	AudioComp->bAllowAnyoneToDestroyMe = true;
	AudioComp->SetWorldLocation(WorldLocation);
	AudioComp->SetSound(*FoundCue);
	AudioComp->SetVolumeMultiplier(BusVolume * FMath::Clamp(Volume, 0.0f, 2.0f));
	AudioComp->bIsUISound = false;
	AudioComp->AttenuationOverrides.bAttenuate = true;
	AudioComp->bOverrideAttenuation = true;
	AudioComp->RegisterComponent();
	AudioComp->Play();
}

void UMistspireAudioSubsystem::SetReverbPreset(FName PresetName, float BlendTime)
{
	UE_LOG(LogMistspire, Log, TEXT("SetReverbPreset: '%s' blend over %.1fs"), *PresetName.ToString(), BlendTime);
}

void UMistspireAudioSubsystem::SetAltitudeEQ(float AltitudeKm)
{
	const float ClampedAlt = FMath::Clamp(AltitudeKm, 0.0f, 12.0f);
	const float Normalized = ClampedAlt / 12.0f;

	float CutoffHz;
	if (Normalized < 0.3f)
	{
		CutoffHz = FMath::Lerp(200.0f, 2000.0f, Normalized / 0.3f);
	}
	else if (Normalized < 0.7f)
	{
		CutoffHz = FMath::Lerp(2000.0f, 8000.0f, (Normalized - 0.3f) / 0.4f);
	}
	else
	{
		CutoffHz = FMath::Lerp(8000.0f, 18000.0f, (Normalized - 0.7f) / 0.3f);
	}

	for (int32 i = 0; i < 6; ++i)
	{
		BusStates[i].FilterCutoffHz = CutoffHz;
	}
}

float UMistspireAudioSubsystem::GetTensionLevel() const
{
	return TensionLevel;
}

void UMistspireAudioSubsystem::SetTensionLevel(float Tension)
{
	TensionLevel = FMath::Clamp(Tension, 0.0f, 1.0f);
}

void UMistspireAudioSubsystem::DebugAudioStats()
{
	const TCHAR* ChannelNames[] = {
		TEXT("Ambient"),
		TEXT("Physiology"),
		TEXT("Weather"),
		TEXT("Surface"),
		TEXT("UI"),
		TEXT("Spatial")
	};

	UE_LOG(LogMistspire, Log, TEXT("=== Mistspire Audio Bus States ==="));
	for (int32 i = 0; i < 6; ++i)
	{
		const FMistspireAudioBusState& Bus = BusStates[i];
		UE_LOG(LogMistspire, Log, TEXT("  [%s] Vol=%.2f Pitch=%.2f Muted=%s Filter=%.0f Hz"),
			ChannelNames[i],
			Bus.VolumeMultiplier,
			Bus.PitchMultiplier,
			Bus.bMuted ? TEXT("YES") : TEXT("no"),
			Bus.FilterCutoffHz);
	}
	UE_LOG(LogMistspire, Log, TEXT("  TensionLevel = %.2f"), TensionLevel);
	UE_LOG(LogMistspire, Log, TEXT("=== End Audio Bus States ==="));
}
