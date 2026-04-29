#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Items/Spells/Spell.h"
#include "BossEnemy.generated.h"

class UGameOverlay;

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDied);

UCLASS()
class LEGENDSOFALDEN3_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()

	public:
		ABossEnemy();

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnBossDied OnBossDied;

	protected:
		/** <AActor> */
		virtual void BeginPlay() override;
		/** </AActor> */

		/* <ABaseCharacter> */
		void SpecialAttack();
		virtual bool CanAttack() override;
		virtual void AttackEnd() override;
		virtual void HandleDamage(float DamageAmount) override;
		/* </ABaseCharacter> */

		/* <AEnemy> */
		virtual void StartAttackTimer() override;
		virtual void ClearAttackTimer() override;
		/* </AEnemy> */

		virtual void Die_Implementation() override;

	private:
		FTimerHandle RecoveryTimer;
		FTimerHandle BossAttackTimer;

		void RecoveryFinished();
		void DecideAttack();
		UGameOverlay* GetGameOverlay() const;

		UPROPERTY(EditAnywhere, Category = "Combat")
		TSubclassOf<ASpell> BossSpellClass;
		UPROPERTY()
		ASpell* BossSpell;
		UPROPERTY(EditAnywhere, Category = "Combat")
		float RecoveryTime = 2.f;
		UPROPERTY(EditAnywhere, Category = "Combat")
		float BossAttackMin = 1.5f;
		UPROPERTY(EditAnywhere, Category = "Combat")
		float BossAttackMax = 3.f;

};
