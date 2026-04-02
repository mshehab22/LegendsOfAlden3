#include "HUD/GameHUD.h"
#include "HUD/GameOverlay.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && GameOverlayClass)
		{
			GameOverlay = CreateWidget<UGameOverlay>(Controller, GameOverlayClass);
			GameOverlay->AddToViewport();
		}
	}
}
