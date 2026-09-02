#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Audio.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireAudioSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMistspireAudioChannel : uint8
{
	Ambient,
	Physiology,
	Weather,
	Surface,
	UI,
	Spatial
};

UENUM(BlueprintType)
enum class EPhysiologySoundType : uint8
{
	HeartbeatNormal,
	HeartbeatRacing,
	BreathingHeavy,
	BreathingShallow,
	Exhaustion,
	AdrenalineSpike,
	HypoxiaGasp
};

USTRUCT(BlueprintType)
struct FMistspireAudioBusState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Bus")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Bus")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Bus")
	bool bMuted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Bus")
	float FilterCutoffHz = 20000.0f;
};

UCLASS()
class MISTSPIRE_API UMistspireAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void SetChannelVolume(EMistspireAudioChannel Channel, float Volume);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void SetChannelPitch(EMistspireAudioChannel Channel, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void MuteChannel(EMistspireAudioChannel Channel, bool bMuted);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Audio")
	float GetChannelVolume(EMistspireAudioChannel Channel) const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void PlayBiomeAmbience(FName BiomeName, float CrossfadeDuration = 2.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void PlayPhysiologySound(EPhysiologySoundType Type, float Intensity = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void PlayWeatherSound(EMistspireWeatherType Weather, float Intensity = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void PlaySurfaceContactSound(float ImpactForce, bool bIsStone = true);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void PlayUISound(FName SoundId, float Volume = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void PlaySpatialSoundAtLocation(FName SoundId, FVector WorldLocation, float Volume = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void SetReverbPreset(FName PresetName, float BlendTime = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void SetAltitudeEQ(float AltitudeKm);

	UFUNCTION(BlueprintPure, Category = "Mistspire|Audio")
	float GetTensionLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Mistspire|Audio")
	void SetTensionLevel(float Tension);

	UFUNCTION(exec, Category = "Mistspire|Audio")
	void DebugAudioStats();

private:
	FMistspireAudioBusState& GetBus(EMistspireAudioChannel Channel);
	UAudioComponent* AcquireOneShotComponent();
	void ResetOneShotComponent(UAudioComponent* Component);

	/** Max pooled one-shot components before the oldest playing voice is recycled. */
	static constexpr int32 MaxOneShotComponents = 12;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	FMistspireAudioBusState BusStates[6];

	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|Audio")
	TMap<FName, USoundCue*> BiomeAmbienceMap;

	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|Audio")
	TMap<EPhysiologySoundType, USoundCue*> PhysiologySoundMap;

	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|Audio")
	TMap<EMistspireWeatherType, USoundCue*> WeatherSoundMap;

	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|Audio")
	TMap<FName, USoundCue*> UISoundMap;

	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|Audio")
	TMap<FName, USoundCue*> SpatialSoundMap;

	UPROPERTY(EditDefaultsOnly, Category = "Mistspire|Audio")
	TMap<FName, USoundCue*> ReverbPresetMap;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	float TensionLevel = 0.f;

	/** Biome whose ambience is currently playing (prevents per-tick restarts). */
	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	FName CurrentAmbienceBiome;

	/** Weather whose loop is currently playing (prevents per-tick restarts). */
	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	EMistspireWeatherType CurrentWeather = EMistspireWeatherType::Clear;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	bool bHasCurrentWeather = false;

	TSet<FName> WarnedMissingBiomeCues;
	TSet<int32> WarnedMissingWeatherCues;
	TSet<int32> WarnedMissingPhysiologyCues;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	UAudioComponent* AmbienceComponent = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	UAudioComponent* WeatherComponent = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	UAudioComponent* PhysiologyComponent = nullptr;

	/** Reusable one-shot components (round-robin pool; oldest voice recycled when full). */
	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	TArray<TObjectPtr<UAudioComponent>> OneShotPool;

	UPROPERTY(VisibleInstanceOnly, Category = "Mistspire|Audio")
	int32 NextOneShotIndex = 0;
};
