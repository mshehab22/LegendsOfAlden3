#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UGameOverlay;

UCLASS()
class LEGENDSOFALDEN3_API AGameHUD : public AHUD
{
	GENERATED_BODY()
	
	public:
		FORCEINLINE UGameOverlay* GetGameOverlay() const { return GameOverlay; }

	protected:
		virtual void BeginPlay() override;

	private:
		UPROPERTY(EditDefaultsOnly, Category = Game)
		TSubclassOf<UGameOverlay> GameOverlayClass;

		UPROPERTY()
		UGameOverlay* GameOverlay;
};
