#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

UCLASS()
class LEGENDSOFALDEN3_API ASoul : public AItem
{
	GENERATED_BODY()
	
	public:
		virtual void Tick(float DeltaTime) override;

		FORCEINLINE int32 GetSouls() const { return Souls; }
		FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }

	protected:
		virtual void BeginPlay() override;

		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	private:
		UPROPERTY(EditAnywhere, Category = "Soul Properties")
		int32 Souls;

		double DesiredZLocation;
		
		UPROPERTY(EditAnywhere)
		float DriftRate = -15.f;
};
