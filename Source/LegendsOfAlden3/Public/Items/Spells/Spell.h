#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Spell.generated.h"

class ASpellProjectile;

UCLASS()
class LEGENDSOFALDEN3_API ASpell : public AItem
{
	GENERATED_BODY()

	public:

		// Spawn behavior
		UPROPERTY(EditAnywhere, Category = "Spell")
		TSubclassOf<ASpellProjectile> ProjectileClass;

		// Cost
		UPROPERTY(EditAnywhere, Category = "Spell")
		float ManaCost = 20.f;

		// Damage
		UPROPERTY(EditAnywhere, Category = "Spell")
		float Damage = 25.f;

		// Casting entry point
		void Cast(class ACharacter* Caster) const;
};