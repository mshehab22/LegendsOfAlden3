#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UAIPerceptionComponent;
class AWeapon;

UCLASS()
class LEGENDSOFALDEN3_API AEnemy : public ACharacterBase
{
	GENERATED_BODY()

	public:
		AEnemy();

		/** <AActor> */
		virtual void Tick(float DeltaTime) override;

		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

		virtual void Destroyed() override;
		/** </AActor> */

		virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

		void SetEnemyState(EEnemyState NewState);

		EEnemyState GetEnemyState() const { return EnemyState; }

	protected:
		/** <AActor> */
		virtual void BeginPlay() override;
		/** </AActor> */

		/* <ABaseCharacter> */
		virtual void Die_Implementation() override;

		void SpawnSoul();

		virtual void LightAttack() override;

		virtual void HeavyAttack() override;

		virtual bool CanAttack() override;

		virtual void AttackEnd() override;

		virtual void HandleDamage(float DamageAmount) override;

		virtual void FinishEquipping()override;

		virtual int32 PlayAttackMontage(UAnimMontage* Montage) override;

		virtual bool CanDisarm() override;

		virtual bool CanArm() override;
		/* </ABaseCharacter> */
		virtual void StartAttackTimer();
		virtual void ClearAttackTimer();

		void MoveToTarget(AActor* Target);

		/* <Combat> */
		void CheckCombatTarget();

		void LoseInterest();

		void ChaseTarget();
		/* </Combat> */

		UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		EEnemyState EnemyState = EEnemyState::EES_Patrolling;

		
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		ECharacterState WeaponType = ECharacterState::ECS_Unequipped;


		UPROPERTY(BlueprintReadOnly)
		EEnemyMovement EnemyMovement = EEnemyMovement::EEM_CanMove;

<<<<<<< Updated upstream
		/* Spell Attack */
		void CastSpellAttack();

		void StartSpellCooldown();

		void ClearSpellCooldown();

		virtual void CastEnd() override;
=======

>>>>>>> Stashed changes

	private:
		/** AI Behavior*/
		void InitializeEnemy();

		void SpawnDefaultWeapon();

		void HideHealthBar();

		void ShowHealthBar();

		void EquipWeapon();

		void UnequipWeapon();

		bool IsChasing();

		bool IsAttacking();

		bool IsDead();

		bool IsEngaged();

		bool IsWeaponInHand();

		bool CanMove();

		/** Patrol */
		void CheckPatrolTarget();

		void PatrolTimerFinished();

		void StartPatrolling();

		void ClearPatrolTimer();

		AActor* ChoosePatrolTarget();


		//void Attack();

		bool IsOutsideCombatRadius();

		bool IsOutsideAttackRadius();

		bool IsInsideAttackRadius();

		bool InTargetRange(AActor* Target, double Radius);

		UFUNCTION()
		void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus); // Callback for OnTargetPerceptionUpdated in UAIPerceptionComponent

		/** Components */
		UPROPERTY(VisibleAnywhere)
		UHealthBarComponent* HealthBarWidget;


		UPROPERTY(VisibleAnywhere)
		UAIPerceptionComponent* AIPerception;


		UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;


		UPROPERTY(EditAnywhere, Category = Combat)
		double CombatRadius = 1000.f;


		UPROPERTY(EditAnywhere, Category = Combat)
		double AttackRadius = 150.f;

		UPROPERTY(EditAnywhere, Category = Combat)
		double AcceptanceRadius = 50.f;
		
		/* Navigation */
		UPROPERTY()
		class AAIController* EnemyController;

		// Current patrol target
		UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		AActor* PatrolTarget;


		UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<AActor*> PatrolTargets;


		UPROPERTY(EditAnywhere, Category = "AI Navigation")
		double PatrolRadius = 200.f;


		FTimerHandle PatrolTimer;

		UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMin = 5.f;

		UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMax = 10.f;


		UPROPERTY(EditAnywhere, Category = "Combat")
		float PatrollingSpeed = 130.f;


		FTimerHandle AttackTimer;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float AttackMin = 0.5f;


		UPROPERTY(EditAnywhere, Category = "Combat")
		float AttackMax = 1.f;


		UPROPERTY(EditAnywhere, Category = "Combat")
		float ChasingSpeed = 300.f;


		UPROPERTY(EditAnywhere, Category = "Combat")
		float DeathLifeSpan = 50.f;


		UPROPERTY(EditAnywhere, Category = "Combat")
		TSubclassOf<class ASoul> SoulClass;

		FTimerHandle SpellCooldownTimer;

		bool bSpellOnCooldown = false;

		UPROPERTY(EditAnywhere, Category = "Combat|Spells")
		float SpellCooldownTime = 5.f;

		UPROPERTY(EditAnywhere, Category = "Combat|Spells", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float SpellCastChance = 0.3f;


		UPROPERTY()
		bool bIsWeaponInHand = false;
};

