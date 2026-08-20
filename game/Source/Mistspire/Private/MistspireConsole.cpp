#include "MistspireAltitudeDebugSubsystem.h"
#include "MistspireAltitudeSubsystem.h"
#include "MistspireEnvironmentSubsystem.h"
#include "MistspireProgressSubsystem.h"
#include "MistspireWorldAtlasSubsystem.h"
#include "MistspireInteriorSubsystem.h"
#include "MistspireVRPawn.h"
#include "MistspireAudioSubsystem.h"
#include "MistspireDialogueSubsystem.h"
#include "MistspireObservationRecorder.h"
#include "MistspireEntitySubsystem.h"
#include "MistspireStateMachine.h"
#include "MistspireAIController.h"
#include "MistspireGOAP.h"
#include "MistspireWanderingGhost.h"
#include "Engine/World.h"
#include "EngineUtils.h"
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

static void MistspireTeleportDistrict(const TArray<FString>& Args)
{
	if (!GWorld || Args.Num() < 1)
	{
		return;
	}
	const int32 Index = FMath::Clamp(FCString::Atoi(*Args[0]), 0, 11);
	if (UMistspireWorldAtlasSubsystem* Atlas = GWorld->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		const TArray<FMistspireDistrictEntry>& Districts = Atlas->GetDistricts();
		if (Districts.IsValidIndex(Index))
		{
			if (APawn* Pawn = GWorld->GetFirstPlayerController()->GetPawn())
			{
				const FVector Target = Districts[Index].BoundsCenter + FVector(0, 0, 25000.f);
				Pawn->SetActorLocation(Target, false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}

static void MistspireExitInterior(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(GWorld->GetFirstPlayerController()->GetPawn()))
	{
		if (UMistspireInteriorSubsystem* Interior = GWorld->GetSubsystem<UMistspireInteriorSubsystem>())
		{
			Interior->ExitBuilding(Pawn);
		}
	}
}

static void MistspireRespawnWorldMarkers(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UMistspireWorldAtlasSubsystem* Atlas = GWorld->GetSubsystem<UMistspireWorldAtlasSubsystem>())
	{
		Atlas->SpawnAuthoredWorldMarkers();
	}
}

static FAutoConsoleCommand CmdMistspireTeleportDistrict(
	TEXT("mistspire.TeleportDistrict"),
	TEXT("Teleport to district index 0-11. Usage: mistspire.TeleportDistrict 0"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireTeleportDistrict));

static FAutoConsoleCommand CmdMistspireExitInterior(
	TEXT("mistspire.ExitInterior"),
	TEXT("Force exit current building interior."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireExitInterior));

static FAutoConsoleCommand CmdMistspireRespawnWorldMarkers(
	TEXT("mistspire.RespawnWorldMarkers"),
	TEXT("Respawn building doors and POI markers from atlas."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireRespawnWorldMarkers));

static void MistspireShowAltitudeHUD(const TArray<FString>& Args)
{
	if (!GWorld) return;
	if (AMistspireVRPawn* Pawn = Cast<AMistspireVRPawn>(GWorld->GetFirstPlayerController()->GetPawn()))
	{
		uint8 Show = 1;
		if (Args.Num() > 0) Show = FMath::Clamp(FCString::Atoi(*Args[0]), 0, 1);
		if (Pawn->AltimeterText) Pawn->AltimeterText->SetHiddenInGame(Show == 0);
	}
}

static FAutoConsoleCommand CmdMistspireShowAltitudeHUD(
	TEXT("mistspire.ShowAltitudeHUD"),
	TEXT("Toggle altitude HUD visibility: 0=hide, 1=show."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireShowAltitudeHUD));

static void MistspireAudioDebug(const TArray<FString>&)
{
	if (!GWorld) return;
	if (UMistspireAudioSubsystem* Audio = GWorld->GetSubsystem<UMistspireAudioSubsystem>())
	{
		Audio->DebugAudioStats();
	}
}

static FAutoConsoleCommand CmdMistspireAudioDebug(
	TEXT("mistspire.DebugAudioStats"),
	TEXT("Log all audio bus states (volume, pitch, mute, filter)."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireAudioDebug));

// ---------------------------------------------------------------------------
// AI / Dialogue / RL debug
// ---------------------------------------------------------------------------

static void MistspireAIThink(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	AMistspireAIController* First = nullptr;
	for (TActorIterator<AMistspireAIController> It(GWorld); It; ++It)
	{
		First = *It;
		break;
	}
	if (!First)
	{
		UE_LOG(LogTemp, Log, TEXT("Mistspire AI: no AMistspireAIController in the world. Spawn one or use SpawnGhostSim."));
		return;
	}

	FMistspireAIWorldState State;
	const APawn* Pawn = GWorld->GetFirstPlayerController() ? GWorld->GetFirstPlayerController()->GetPawn() : nullptr;
	if (Pawn)
	{
		State.AltitudeCm = Pawn->GetActorLocation().Z;
	}
	if (const AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(Pawn))
	{
		State.Oxygen01 = VRPawn->GetOxygenPercent();
		State.Stamina01 = VRPawn->GetStaminaPercent();
	}
	First->UpdateWorldState(State);
	const FMistspireUtilityDecision Decision = First->RunUtilityDecision();
	UE_LOG(LogTemp, Log, TEXT("Mistspire AI: utility decision = %s (%.2f)"),
		Decision.bValid ? *Decision.DecisionName.ToString() : TEXT("none"), Decision.Score);
}

static FAutoConsoleCommand CmdMistspireAIThink(
	TEXT("mistspire.AIThink"),
	TEXT("Run one utility-AI + GOAP decision for the first AI controller."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireAIThink));

static void MistspireGOAPPlan(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}

	FMistspireGOAPState Goal;
	if (Args.Num() > 0)
	{
		Goal.Facts.FindOrAdd(FName(*Args[0])) = true;
	}
	else
	{
		Goal.Facts.FindOrAdd(TEXT("BeaconReached")) = true;
	}

	FMistspireAIWorldState State;
	const APawn* Pawn = GWorld->GetFirstPlayerController() ? GWorld->GetFirstPlayerController()->GetPawn() : nullptr;
	if (Pawn)
	{
		State.AltitudeCm = Pawn->GetActorLocation().Z;
	}
	if (const AMistspireVRPawn* VRPawn = Cast<AMistspireVRPawn>(Pawn))
	{
		State.Oxygen01 = VRPawn->GetOxygenPercent();
		State.Stamina01 = VRPawn->GetStaminaPercent();
	}

	FMistspireGOAPState Start;
	Start.Facts.FindOrAdd(TEXT("HasOxygen")) = State.Oxygen01 > 0.1f;
	Start.Facts.FindOrAdd(TEXT("HasStamina")) = State.Stamina01 > 0.1f;
	Start.Facts.FindOrAdd(TEXT("CanisterNearby")) = true;
	Start.Facts.FindOrAdd(TEXT("ShelterKnown")) = true;

	const TArray<FMistspireGOAPAction> Actions = UMistspireGOAPPlanner::BuildMistspireActionLibrary();
	TArray<FMistspireGOAPAction> Plan;
	if (UMistspireGOAPPlanner::Plan(Start, Goal, Actions, Plan))
	{
		FString PlanText;
		for (const FMistspireGOAPAction& Action : Plan)
		{
			if (!PlanText.IsEmpty())
			{
				PlanText += TEXT(" -> ");
			}
			PlanText += Action.ActionName.ToString();
		}
		UE_LOG(LogTemp, Log, TEXT("Mistspire GOAP: plan [%s] -> %s"), *PlanText,
			PlanText.IsEmpty() ? TEXT("(goal already satisfied)") : *PlanText);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Mistspire GOAP: no plan found for goal '%s'"), *Goal.ToKeyString());
	}
}

static FAutoConsoleCommand CmdMistspireGOAPPlan(
	TEXT("mistspire.GOAPPlan"),
	TEXT("Print the GOAP plan for a goal fact. Usage: mistspire.GOAPPlan BeaconReached"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireGOAPPlan));

static void MistspireSpeak(const TArray<FString>& Args)
{
	if (!GWorld || Args.Num() < 1)
	{
		return;
	}
	if (UMistspireDialogueSubsystem* Dialogue = GWorld->GetSubsystem<UMistspireDialogueSubsystem>())
	{
		Dialogue->Speak(FName(*Args[0]));
	}
}

static FAutoConsoleCommand CmdMistspireSpeak(
	TEXT("mistspire.Speak"),
	TEXT("Play a dialogue line by id. Usage: mistspire.Speak companion_greeting"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireSpeak));

static void MistspireObservationStart(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}
	if (UMistspireObservationRecorder* Recorder = GWorld->GetSubsystem<UMistspireObservationRecorder>())
	{
		float Interval = 1.f;
		if (Args.Num() > 0)
		{
			Interval = FCString::Atof(*Args[0]);
		}
		Recorder->StartRecording(Interval);
	}
}

static FAutoConsoleCommand CmdMistspireObservationStart(
	TEXT("mistspire.ObservationStart"),
	TEXT("Start RL observation CSV recording. Usage: mistspire.ObservationStart [intervalSeconds=1]"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireObservationStart));

static void MistspireObservationStop(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	if (UMistspireObservationRecorder* Recorder = GWorld->GetSubsystem<UMistspireObservationRecorder>())
	{
		Recorder->StopRecording();
	}
}

static FAutoConsoleCommand CmdMistspireObservationStop(
	TEXT("mistspire.ObservationStop"),
	TEXT("Stop RL observation CSV recording."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireObservationStop));

static void MistspireSpawnGhostSim(const TArray<FString>& Args)
{
	if (!GWorld)
	{
		return;
	}
	FVector SpawnLocation = FVector::ZeroVector;
	const APawn* Pawn = GWorld->GetFirstPlayerController() ? GWorld->GetFirstPlayerController()->GetPawn() : nullptr;
	if (Pawn)
	{
		SpawnLocation = Pawn->GetActorLocation() + FVector(400.f, 400.f, 1200.f);
	}
	if (Args.Num() >= 3)
	{
		SpawnLocation = FVector(FCString::Atof(*Args[0]), FCString::Atof(*Args[1]), FCString::Atof(*Args[2]));
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMistspireWanderingGhost* Ghost = GWorld->SpawnActor<AMistspireWanderingGhost>(SpawnLocation, FRotator::ZeroRotator, Params);
	if (Ghost)
	{
		UE_LOG(LogTemp, Log, TEXT("Mistspire AI: spawned wandering ghost at %s"),
			*SpawnLocation.ToString());
	}
}

static FAutoConsoleCommand CmdMistspireSpawnGhostSim(
	TEXT("mistspire.SpawnGhostSim"),
	TEXT("Spawn a steering+FSM ghost at the player or [x y z]."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireSpawnGhostSim));

static void MistspireStateMachineDebug(const TArray<FString>&)
{
	if (!GWorld)
	{
		return;
	}
	TArray<UActorComponent*> Components = GWorld->GetComponentsByClass(UMistspireStateMachineComponent::StaticClass());
	int32 Count = 0;
	for (const UActorComponent* Component : Components)
	{
		const UMistspireStateMachineComponent* FSM = Cast<UMistspireStateMachineComponent>(Component);
		if (FSM)
		{
			UE_LOG(LogTemp, Log, TEXT("Mistspire FSM: %s.%s = %s"),
				*GetNameSafe(FSM->GetOwner()), *FSM->GetName(), *FSM->GetCurrentState().ToString());
			++Count;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Mistspire FSM: %d active state machines."), Count);
}

static FAutoConsoleCommand CmdMistspireStateMachineDebug(
	TEXT("mistspire.StateMachineDebug"),
	TEXT("Log active state of every Mistspire state machine component."),
	FConsoleCommandWithArgsDelegate::CreateStatic(&MistspireStateMachineDebug));
