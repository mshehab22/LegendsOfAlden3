#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class LEGENDSOFALDEN3_API UGameOverlay : public UUserWidget
{
	GENERATED_BODY()
	
	public:
		void SetHealthBarPercent(float Percent);
		void SetManaBarPercent(float Percent);

		void SetBossHealthBarPercent(float Percent);
		void SetBossHealthBarVisible(bool bVisible);

	private:
		UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthProgressBar;


		UPROPERTY(meta = (BindWidget))
		UProgressBar* ManaProgressBar;

		UPROPERTY(meta = (BindWidget))
		UProgressBar* BossHealthProgressBar;

};
