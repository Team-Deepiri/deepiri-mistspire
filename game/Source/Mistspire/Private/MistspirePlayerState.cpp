#include "MistspirePlayerState.h"
#include "Net/UnrealNetwork.h"

AMistspirePlayerState::AMistspirePlayerState()
{
	bReplicates = true;
}

void AMistspirePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutReplicatedProps) const
{
	Super::GetLifetimeReplicatedProps(OutReplicatedProps);

	DOREPLIFETIME(AMistspirePlayerState, MaxAltitudeCm);
	DOREPLIFETIME(AMistspirePlayerState, ReachedSummits);
}

void AMistspirePlayerState::UpdateAltitude(float NewAltitude)
{
	if (NewAltitude > MaxAltitudeCm)
	{
		MaxAltitudeCm = NewAltitude;
	}
}

void AMistspirePlayerState::AddSummit(FName SummitId)
{
	ReachedSummits.AddUnique(SummitId);
}
