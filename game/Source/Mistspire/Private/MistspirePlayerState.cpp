#include "MistspirePlayerState.h"
#include "MistspireGameState.h"
#include "Net/UnrealNetwork.h"

AMistspirePlayerState::AMistspirePlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 5.f; // Low frequency for player state stats
}

void AMistspirePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const
{
	Super::GetLifetimeReplicatedProps(OutReplicatedProps);

	DOREPLIFETIME(AMistspirePlayerState, MaxAltitudeCm);
	DOREPLIFETIME(AMistspirePlayerState, CurrentAltitudeCm);
	DOREPLIFETIME(AMistspirePlayerState, ReachedSummits);
}

void AMistspirePlayerState::UpdateAltitude(float NewAltitude)
{
	CurrentAltitudeCm = NewAltitude;
	if (NewAltitude > MaxAltitudeCm)
	{
		MaxAltitudeCm = NewAltitude;
	}

	if (GetNetMode() < NM_DedicatedServer)
	{
		Server_SyncAltitudeToGameState(CurrentAltitudeCm, MaxAltitudeCm);
	}
}

void AMistspirePlayerState::AddSummit(FName SummitId)
{
	ReachedSummits.AddUnique(SummitId);
}

bool AMistspirePlayerState::Server_SyncAltitudeToGameState_Validate(float Current, float Max) { return true; }
void AMistspirePlayerState::Server_SyncAltitudeToGameState_Implementation(float Current, float Max)
{
	if (AMistspireGameState* GS = GetWorld()->GetGameState<AMistspireGameState>())
	{
		GS->NotifyAltitudeSample(GetPlayerName(), Current, Max);
	}
}
