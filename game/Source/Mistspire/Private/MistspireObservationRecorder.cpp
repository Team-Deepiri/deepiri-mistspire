#include "MistspireObservationRecorder.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireZoneSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireBeaconSubsystem.h"
#include "MistspireEntitySubsystem.h"
#include "MistspireVRPawn.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Misc/Char.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"

TStatId UMistspireObservationRecorder::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMistspireObservationRecorder, STATGROUP_Tickables);
}

FString UMistspireObservationRecorder::GetOutputPath() const
{
	return FPaths::ProjectSavedDir() / TEXT("RL") / TEXT("observations.csv");
}

void UMistspireObservationRecorder::EnsureFileOpen()
{
	if (FileHandle)
	{
		return;
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString Directory = FPaths::ProjectSavedDir() / TEXT("RL");
	PlatformFile.CreateDirectoryTree(*Directory);

	FileHandle = PlatformFile.OpenWrite(*GetOutputPath(), /*bAppend=*/true);
	if (FileHandle)
	{
		// Write the header if the file was just created (empty).
		if (FileHandle->Size() == 0)
		{
			const FString Header = TEXT("AltitudeCm,PersonalBestCm,Oxygen01,Stamina01,Pressure01,WeatherIndex,ZoneIndex,BiomeIndex,BeaconDistanceKm,Exposed01,IsClimbing01,IsGliding01,GhostCount,EntityCount\n");
			const FTCHARToUTF8 Converter(*Header);
			FileHandle->Write(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length());
		}
	}
}

void UMistspireObservationRecorder::StartRecording(float SampleIntervalSeconds)
{
	bRecording = true;
	this->RecordingIntervalSeconds = FMath::Max(SampleIntervalSeconds, 0.05f);
	Accumulator = 0.f;
	RowsWritten = 0;
	EnsureFileOpen();
	UE_LOG(LogTemp, Log, TEXT("Mistspire RL: recording observations -> %s"), *GetOutputPath());
	SampleNow();
}

void UMistspireObservationRecorder::StopRecording()
{
	bRecording = false;
	if (FileHandle)
	{
		delete FileHandle;
		FileHandle = nullptr;
	}
	UE_LOG(LogTemp, Log, TEXT("Mistspire RL: stopped after %d rows."), RowsWritten);
}

void UMistspireObservationRecorder::CollectFeatures(TArray<float>& OutFeatures) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	const AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(Pawn);

	const UMistspireAltitudeSubsystem* Altitude = World->GetSubsystem<UMistspireAltitudeSubsystem>();
	const UMistspireZoneSubsystem* Zone = World->GetSubsystem<UMistspireZoneSubsystem>();
	const UMistspireEnvironmentSubsystem* Environment = World->GetSubsystem<UMistspireEnvironmentSubsystem>();
	const UMistspireBeaconSubsystem* Beacon = World->GetSubsystem<UMistspireBeaconSubsystem>();
	const UMistspireEntitySubsystem* Entities = World->GetSubsystem<UMistspireEntitySubsystem>();

	const float AltitudeCm = Altitude ? Altitude->GetCurrentAltitudeCm() : 0.f;
	const float PersonalBestCm = Altitude ? Altitude->GetPersonalBestAltitudeCm() : 0.f;
	const float Oxygen01 = VRPawn ? VRPawn->GetOxygenPercent() : 1.f;
	const float Stamina01 = VRPawn ? VRPawn->GetStaminaPercent() : 1.f;
	const float Pressure = VRPawn ? VRPawn->GetAtmosphericPressure() : 1.f;
	const uint8 WeatherIndex = Environment ? static_cast<uint8>(Environment->GetCurrentWeather()) : 0;
	const uint8 ZoneIndex = Zone ? static_cast<uint8>(Zone->GetCurrentZone()) : 0;
	const uint8 BiomeIndex = Environment ? static_cast<uint8>(Environment->GetCurrentBiome()) : 0;
	const float BeaconKm = Beacon ? Beacon->GetCachedBeacon().DistanceCm / 100000.f : 0.f;
	const bool bExposed = WeatherIndex >= 2 && AltitudeCm > 300000.f;
	const bool bClimbing = VRPawn ? VRPawn->bIsClimbing : false;
	const bool bGliding = VRPawn ? VRPawn->bGliderActive : false;
	const int32 GhostCount = Entities ? Entities->QueryIds(TEXT("Ghost")).Num() : 0;
	const int32 EntityCount = Entities ? Entities->GetEntityCount() : 0;

	OutFeatures.Add(AltitudeCm);
	OutFeatures.Add(PersonalBestCm);
	OutFeatures.Add(Oxygen01);
	OutFeatures.Add(Stamina01);
	OutFeatures.Add(FMath::Clamp(Pressure, 0.f, 1.4f));
	OutFeatures.Add(static_cast<float>(WeatherIndex));
	OutFeatures.Add(static_cast<float>(ZoneIndex));
	OutFeatures.Add(static_cast<float>(BiomeIndex));
	OutFeatures.Add(BeaconKm);
	OutFeatures.Add(bExposed ? 1.f : 0.f);
	OutFeatures.Add(bClimbing ? 1.f : 0.f);
	OutFeatures.Add(bGliding ? 1.f : 0.f);
	OutFeatures.Add(static_cast<float>(GhostCount));
	OutFeatures.Add(static_cast<float>(EntityCount));
}

void UMistspireObservationRecorder::SampleNow()
{
	EnsureFileOpen();
	if (!FileHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mistspire RL: cannot open observation CSV."));
		return;
	}

	TArray<float> Features;
	CollectFeatures(Features);

	FString Row;
	for (int32 Idx = 0; Idx < Features.Num(); ++Idx)
	{
		Row += FString::Printf(TEXT("%.1f%s"), Features[Idx], Idx + 1 < Features.Num() ? TEXT(",") : TEXT("\n"));
	}

	const FTCHARToUTF8 Converter(*Row);
	FileHandle->Write(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length());
	++RowsWritten;
}

void UMistspireObservationRecorder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bRecording)
	{
		return;
	}
	Accumulator += DeltaTime;
	if (Accumulator >= RecordingIntervalSeconds)
	{
		Accumulator = 0.f;
		SampleNow();
	}
}

void UMistspireObservationRecorder::Deinitialize()
{
	if (FileHandle)
	{
		delete FileHandle;
		FileHandle = nullptr;
	}
	Super::Deinitialize();
}