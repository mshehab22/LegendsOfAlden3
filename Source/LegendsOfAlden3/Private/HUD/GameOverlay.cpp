#include "HUD/GameOverlay.h"
#include "Components/ProgressBar.h"

void UGameOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UGameOverlay::SetManaBarPercent(float Percent)
{
	if (ManaProgressBar)
	{
		ManaProgressBar->SetPercent(Percent);
	}
}
