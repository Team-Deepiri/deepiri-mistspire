#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
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

static FAutoConsoleCommand CmdMistspireAltitudeStats(
	TEXT("mistspire.AltitudeStats"),
	TEXT("Log current and personal-best altitude."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireAltitudeStats));

static FAutoConsoleCommand CmdMistspireTeleportUp(
	TEXT("mistspire.TeleportUp"),
	TEXT("Teleport player up Z (cm). Usage: mistspire.TeleportUp 5000"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireTeleportUp));
