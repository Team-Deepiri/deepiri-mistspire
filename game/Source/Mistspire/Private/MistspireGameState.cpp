#include "MistspireGameState.h"

void AMistspireGameState::NotifyAltitudeSample(float AltitudeCm)
{
	if (AltitudeCm > SessionBestAltitudeCm)
	{
		SessionBestAltitudeCm = AltitudeCm;
	}
}
