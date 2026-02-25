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

	protected:

		/** <AActor> */
		virtual void BeginPlay() override;
		/** </AActor> */

		virtual void LightAttack();
		virtual void HeavyAttack();
		virtual void Die();
		void DirectionalHitReact(const FVector& ImpactPoint);

		virtual void HandleDamage(float DamageAmount);
		void PlayHitSound(const FVector& ImpactPoint);
		void SpawnHitParticles(const FVector& ImpactPoint);
		void DisableCapsule();
		virtual bool CanAttack();
		bool IsAlive();
		virtual bool CanDisarm();
		virtual bool CanArm();
		virtual bool CanMove();

		/** Play montage functions */
		void PlayHitReactMontage(const FName& SectionName);
		virtual int32 PlayAttackMontage(UAnimMontage* Montage);
		virtual int32 PlayDeathMontage();
		void PlayEquipMontage(const FName& SectionName);
		/** /Play montage functions */

		UFUNCTION(BlueprintCallable)
		virtual void AttackEnd();

		UFUNCTION(BlueprintCallable)
		void AttachWeaponToBack();

		UFUNCTION(BlueprintCallable)
		void AttachWeaponToHand();

		UFUNCTION(BlueprintCallable)
		virtual void FinishEquipping();

		UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

		UPROPERTY(VisibleAnywhere, Category = Weapon)
		AWeapon* EquippedWeapon;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAttributeComponent* Attributes;

		int32 LastSelectionIndex = -1;

	private:
		/** Play montage functions */
		void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
		int32 PlayRandomMontageSection(UAnimMontage* Montage);

		UPROPERTY(EditAnywhere, Category = Sounds)
		USoundBase* HitSound;

		UPROPERTY(EditAnywhere, Category = VisualEffects)
		UParticleSystem* HitParticles;
	
		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* EquipMontage;

		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* HitReactMontage;

		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* DeathMontage;

};
