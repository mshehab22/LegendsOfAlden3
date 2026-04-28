#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChanged, float, NewPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulsChanged, int32, NewAmount);

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

		float GetHealthPercent() const;

		float GetManaPercent() const;

		bool IsAlive() const;

		void AddGold(int32 AmountOfGold);

		void AddSouls(int32 NumberOfSouls);

		UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnHealthChanged OnHealthChanged;

		UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnManaChanged OnManaChanged;

		UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnGoldChanged OnGoldChanged;

		UPROPERTY(BlueprintAssignable, Category = "Attributes|Events")
		FOnSoulsChanged OnSoulsChanged;

		FORCEINLINE int32 GetGold() const { return Gold; }

		FORCEINLINE int32 GetSouls() const { return Souls; }

		FORCEINLINE float GetMana() const { return Mana; }



	protected:
		virtual void BeginPlay() override;

	private:
		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Health = 100.f;

		UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxHealth = 100.f;

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
 