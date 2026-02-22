#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


class USphereComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
	EIS_World,
	EIS_Equipped,
	EIS_Inventory
};

UCLASS()
class LEGENDSOFALDEN3_API AItem : public AActor
{
	GENERATED_BODY()
	
	public:	
		AItem();

		virtual void Tick(float DeltaTime) override;

	protected:
		virtual void BeginPlay() override;

		template<typename T> 
		T Average(T First, T Second);

		UFUNCTION()
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly) 
		UStaticMeshComponent* ItemMesh;

		UPROPERTY(VisibleAnywhere)
		USphereComponent* Sphere;

		EItemState ItemState = EItemState::EIS_World;

		UPROPERTY(EditAnywhere)
		UNiagaraComponent* EmbersEffect;

	private:

};

template<typename T>
inline T AItem::Average(T First, T Second)
{
	return (First + Second) / 2;
}
