#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEGENDSOFALDEN3_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

	public:	
		UAttributeComponent();

		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		void RegenerateMana(float DeltaTime);

		void ReceiveDamage(float Damage);

		void ManaUsage(float ManaCost);

		float GetHealthPercent();

		float GetManaPercent();

		bool IsAlive();

		void AddGold(int32 AmountOfGold);

		void AddSouls(int32 NumberOfSouls);


		FORCEINLINE int32 GetGold() const { return Gold; }

		FORCEINLINE int32 GetSouls() const { return Souls; }

		FORCEINLINE float GetMana() const { return Mana; }

	protected:
		virtual void BeginPlay() override;

	private:
		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Health;

		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxHealth;

		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Mana;

		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxMana;

		UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
		int32 Gold;

		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Souls;

		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float ManaRegenerationRate = 8.f;
};
 