#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterBase.generated.h"

class UAnimMontage;
class AWeapon;
class UAttributeComponent;

UCLASS()
class LEGENDSOFALDEN3_API ACharacterBase : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

	public:
		ACharacterBase();
		virtual void Tick(float DeltaTime) override;

		UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	protected:
		virtual void BeginPlay() override;
		virtual void LightAttack();
		virtual void HeavyAttack();
		virtual void Die();

		// Play montage functions
		virtual void PlayAttackMontage(UAnimMontage* MontageToPlay);
		virtual void PlayEquipMontage(const FName& SectionName);
		void PlayHitReactMontage(const FName& SectionName);
		void DirectionalHitReact(const FVector& ImpactPoint);
		
		UFUNCTION(BlueprintCallable)
		virtual void AttackEnd();
		
		virtual bool CanAttack();

		virtual bool CanDisarm();

		virtual bool CanArm();

		virtual bool CanMove();

		UFUNCTION(BlueprintCallable)
		void Disarm();

		UFUNCTION(BlueprintCallable)
		void Arm();

		UFUNCTION(BlueprintCallable)
		virtual void FinishEquipping();


		UPROPERTY(VisibleAnywhere, Category = Weapon)
		AWeapon* EquippedWeapon;

		// Animation montages
		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* AttackMontage;

		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* EquipMontage;

		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* HitReactMontage;

		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* DeathMontage;

		/**
		* Components
		*/
		UPROPERTY(VisibleAnywhere)
		UAttributeComponent* Attributes;

		UPROPERTY(EditAnywhere, Category = Sounds)
		USoundBase* HitSound;

		UPROPERTY(EditAnywhere, Category = VisualEffects)
		UParticleSystem* HitParticles;

	private:	


};
