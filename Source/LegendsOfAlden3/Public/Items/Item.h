#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class LEGENDSOFALDEN3_API AItem : public AActor
{
	GENERATED_BODY()
	
	public:	
		AItem();

		virtual void Tick(float DeltaTime) override;

	protected:
		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters") float Amplitude = 0.25f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters") float TimeConstant = 5.f;

		UFUNCTION(BlueprintPure) float TransformedSin();

		UFUNCTION(BlueprintPure) float TransformedCos();

		template<typename T> T Average(T First, T Second);

	private:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) float RunningTime;


		UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ItemMesh;

};

template<typename T>
inline T AItem::Average(T First, T Second)
{
	return (First + Second) / 2;
}
