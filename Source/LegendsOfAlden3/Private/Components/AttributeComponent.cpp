#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(GetHealthPercent());
}

void UAttributeComponent::ManaUsage(float ManaCost)
{
	Mana = FMath::Clamp(Mana - ManaCost, 0.f, MaxMana);
	OnManaChanged.Broadcast(GetManaPercent());
}

float UAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetManaPercent() const
{
	return Mana / MaxMana;
}

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
	OnGoldChanged.Broadcast(Gold);
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
	OnSoulsChanged.Broadcast(Souls);
}
 

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::RegenerateMana(float DeltaTime)
{
	const float OldMana = Mana;
	Mana = FMath::Clamp(Mana + ManaRegenerationRate * DeltaTime, 0.f, MaxMana);

	if (!FMath::IsNearlyEqual(OldMana, Mana))
	{
		OnManaChanged.Broadcast(GetManaPercent());
	}
}

