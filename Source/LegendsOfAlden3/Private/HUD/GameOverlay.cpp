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

void UGameOverlay::SetBossHealthBarPercent(float Percent)
{
	if (BossHealthProgressBar) BossHealthProgressBar->SetPercent(Percent);
}

void UGameOverlay::SetBossHealthBarVisible(bool bVisible)
{
	if (BossHealthProgressBar) BossHealthProgressBar->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
