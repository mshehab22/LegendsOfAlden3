#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "CharacterBase.generated.h"

class UAnimMontage;
class AWeapon;
class UAttributeComponent;
class ASpell;
class ASpellProjectile;

UCLASS()
class LEGENDSOFALDEN3_API ACharacterBase : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

	public:
		ACharacterBase();

		virtual void Tick(float DeltaTime) override;

		FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

	protected:
		virtual void BeginPlay() override;

		virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

		/* Combat */
		virtual void LightAttack();

		virtual void HeavyAttack();

		UFUNCTION(BlueprintNativeEvent)
		void Die();

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

		void DisableMeshCollision();

		/* Spell System */
		UFUNCTION(BlueprintCallable)
		virtual void SpawnSpell();

		UFUNCTION(BlueprintCallable)
		virtual void LaunchSpell();

		UFUNCTION(BlueprintCallable)
		virtual void CastEnd();

		UPROPERTY(EditAnywhere, Category = "Spell")
		TSubclassOf<ASpell> EquippedSpellClass;

		UPROPERTY(EditDefaultsOnly, Category = "Spell")
		UAnimMontage* SpellCastMontage;

		UPROPERTY()
		ASpellProjectile* PendingSpellProjectile = nullptr;

		FRotator CastRotation;

		/* Play montage functions */
		void PlayHitReactMontage(const FName& SectionName);

		virtual int32 PlayAttackMontage(UAnimMontage* Montage);

		virtual int32 PlayDeathMontage();

		void PlayEquipMontage(const FName& SectionName);

		virtual void PlayDodgeMontage(const FName& SectionName);

		void StopAttackMontage();

		UFUNCTION(BlueprintCallable)
		FVector GetTranslationWarpTarget();

		
		UFUNCTION(BlueprintCallable)
		FVector GetRotationWarpTarget();


		/* Play montage functions */
		UFUNCTION(BlueprintCallable)
		virtual void AttackEnd(); 


		UFUNCTION(BlueprintCallable)
		virtual void DodgeEnd(); 


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


		UPROPERTY(BlueprintReadOnly, Category = "Combat")
		AActor* CombatTarget;


		UPROPERTY(EditAnywhere, Category = "Combat")
		double WarpTargetDistance = 75.f;


		UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EDeathPose> DeathPose;

		int32 LastSelectionIndex = -1;

	private:
		/* Play montage functions */
		void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

		int32 PlayRandomMontageSection(UAnimMontage* Montage); // , const TArray<FName>& SectionNames

		UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* HitSound;


		UPROPERTY(EditAnywhere, Category = Combat)
		UParticleSystem* HitParticles;

	
		UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* EquipMontage;


		UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* HitReactMontage;


		UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DeathMontage;
		

		UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DodgeMontage;

};
