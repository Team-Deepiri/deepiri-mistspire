#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireProgressSubsystem.h"
#include "MistspireVRPawn.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"

static void MistspireAltitudeStats(const TArray<FString>& Args)
{
	if (UWorld* World = GWorld)
	{
		if (UMistspireAltitudeDebugSubsystem* Dbg = World->GetSubsystem<UMistspireAltitudeDebugSubsystem>())
		{
			Dbg->LogAltitudeStats();
		}
	}
}

static void MistspireTeleportUp(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}

	float DeltaCm = 5000.f;
	if (Args.Num() > 0)
	{
		DeltaCm = FCString::Atof(*Args[0]);
	}

	if (APawn* Pawn = GWorld->GetFirstPlayerController() ? GWorld->GetFirstPlayerController()->GetPawn() : nullptr)
	{
		Pawn->AddActorWorldOffset(FVector(0.f, 0.f, DeltaCm), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

static void MistspireSetWeather(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}
	UMistspireEnvironmentSubsystem* Env = GWorld->GetSubsystem<UMistspireEnvironmentSubsystem>();
	if (!Env)
	{
		return;
	}

	int32 Index = 0;
	if (Args.Num() > 0)
	{
		Index = FCString::Atoi(*Args[0]);
	}
	Index = FMath::Clamp(Index, 0, 3);
	Env->ForceWeather(static_cast<EMistspireWeatherType>(Index), 120.f);
}

static void MistspireRefillSurvival(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(GWorld->GetFirstPlayerController()->GetPawn()))
	{
		Pawn->ApplyShelterRefill(100.f, 100.f, 1.f);
	}
}

static FAutoConsoleCommand CmdMistspireAltitudeStats(
	TEXT("mistspire.AltitudeStats"),
	TEXT("Log current and personal-best altitude."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireAltitudeStats));

static FAutoConsoleCommand CmdMistspireTeleportUp(
	TEXT("mistspire.TeleportUp"),
	TEXT("Teleport player up Z (cm). Usage: mistspire.TeleportUp 5000"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireTeleportUp));

static FAutoConsoleCommand CmdMistspireSetWeather(
	TEXT("mistspire.SetWeather"),
	TEXT("Force weather: 0=Clear 1=MistStorm 2=Electric 3=ZenithGlow"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireSetWeather));

static FAutoConsoleCommand CmdMistspireRefillSurvival(
	TEXT("mistspire.RefillSurvival"),
	TEXT("Refill stamina and oxygen (debug)."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireRefillSurvival));

static void MistspireSaveProgress(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UGameInstance* GI = GWorld->GetGameInstance())
	{
		if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
		{
			Progress->CaptureProgressFromWorld(GWorld);
			UE_LOG(LogTemp, Log, TEXT("Mistspire: progress saved."));
		}
	}
}

static void MistspireLoadProgress(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UGameInstance* GI = GWorld->GetGameInstance())
	{
		if (UMistspireProgressSubsystem* Progress = GI->GetSubsystem<UMistspireProgressSubsystem>())
		{
			Progress->LoadProgress();
			Progress->ApplyLoadedProgressToWorld(GWorld);
			UE_LOG(LogTemp, Log, TEXT("Mistspire: progress loaded."));
		}
	}
}

static FAutoConsoleCommand CmdMistspireSaveProgress(
	TEXT("mistspire.SaveProgress"),
	TEXT("Save personal best altitude and summits to disk."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireSaveProgress));

static FAutoConsoleCommand CmdMistspireLoadProgress(
	TEXT("mistspire.LoadProgress"),
	TEXT("Load saved progress into the current world."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireLoadProgress));
