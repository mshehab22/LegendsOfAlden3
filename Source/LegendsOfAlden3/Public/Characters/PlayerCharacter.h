#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class ASoul;
class ATreasure;
class ASpell;
class UGameOverlay;
class UDecalComponent;
class ASpellProjectile;

UCLASS()
class LEGENDSOFALDEN3_API APlayerCharacter : public ACharacterBase, public IPickupInterface
{
	GENERATED_BODY()

	public:
		APlayerCharacter();

		virtual void Tick(float DeltaTime) override;
		
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

		virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

		virtual void SetOverlappingItem(AItem* Item) override;

		virtual void AddSouls(ASoul* Soul) override;

		virtual void AddGold(ATreasure* Gold) override;

		void ToggleLockOn();

		FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
		FORCEINLINE EActionState GetActionState() const { return ActionState; }

	protected:
		virtual void BeginPlay() override;

		/* Callbacks for input */
		void Movement(const FInputActionValue& Value);
		void Look(const FInputActionValue& Value);
		virtual void Jump() override;
		void Interact();
		virtual void LightAttack() override;
		virtual void HeavyAttack() override;
		void Dodge();
		void StartRun();
		void StopRun();

		/* Spell System */
		void CastSpell();
		bool CanCastSpell() const;
		bool HasEnoughMana() const;

		UFUNCTION(BlueprintCallable)
		void SpawnSpell();   // Anim notify: spawns projectile

		UFUNCTION(BlueprintCallable)
		void LaunchSpell();

		UFUNCTION(BlueprintCallable)
		void CastEnd();    // Anim notify at end of montage: releases ActionState


		UPROPERTY(EditAnywhere, Category = "Spell")
		TSubclassOf<ASpell> EquippedSpellClass;

		UPROPERTY(EditDefaultsOnly, Category = "Spell")
		UAnimMontage* SpellCastMontage;

		/* Combat */
		void EquipWeapon(AWeapon* Weapon);
		void Disarm();
		void Arm();
		virtual void AttackEnd() override;
		virtual void DodgeEnd() override;
		virtual bool CanAttack() override;
		virtual bool CanDisarm() override;
		virtual bool CanArm() override;
		virtual bool CanMove() override;
		virtual void FinishEquipping()override;
		virtual int32 PlayAttackMontage(UAnimMontage* Montage) override;
		virtual void Die_Implementation() override;

		UFUNCTION(BlueprintCallable)
		void EnableAttackBuffer();

		UFUNCTION(BlueprintCallable)
		void HitReactEnd();

		UPROPERTY(EditAnywhere, Category = "Movement")
		float WalkSpeed = 200.f;

		UPROPERTY(EditAnywhere, Category = "Movement")
		float RunSpeed = 600.f;

		UPROPERTY(EditAnywhere, Category = "Movement")
		float JumpHeight = 600.f;

		UPROPERTY(EditAnywhere, Category = "Movement")
		float GravityScale = 1.2f;

		// Lock-On State
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
		AActor* LockedTarget = nullptr;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
		bool bIsLockedOn = false;

		UPROPERTY(EditAnywhere, Category = "LockOn")
		float LockOnRadius = 1000.f;

		UPROPERTY(VisibleAnywhere, Category = "LockOn")
		UDecalComponent* LockOnDecal;

		UPROPERTY(EditDefaultsOnly, Category = "LockOn")
		UMaterialInterface* LockOnMaterial;

		void HandleLockOn(float DeltaTime);
		void SetLockOnTarget(AActor* Target);
		void ClearLockOn();
		AActor* FindBestTarget();

	private:
		bool IsUnoccupied();

		bool IsGrounded();

		void InitializeGameOverlay(APlayerController* PlayerController);

		void SetHUDHealth();

		void UpdateHUD(float DeltaTime);

		void UpdateRotationState();

		void InitializeLockOnDecal();

		void UpdateLockOnDecal();


		UFUNCTION()
		void HandleHealthChanged(float NewPercent);

		UFUNCTION()
		void HandleManaChanged(float NewPercent);

		/* Input Variables */
		UPROPERTY(EditAnywhere, Category = Input)
		UInputMappingContext* PlayerCharacterMappingContext;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* MovementAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* RunAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* LookAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* JumpAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* InteractAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* LightAttackAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* HeavyAttackAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* DodgeAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* LockOnAction;


		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* SpellAction;
		/** /Input Variables*/

		/** Character Components */
		UPROPERTY(VisibleAnywhere)
		USpringArmComponent* CameraBoom;


		UPROPERTY(VisibleAnywhere)
		UCameraComponent* ViewCamera;


		UPROPERTY(VisibleInstanceOnly)
		AItem* OverlappingItem;

		
		UPROPERTY()
		UAnimMontage* CurrentAttackMontage = nullptr;
		
		ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

		EBufferedAttackType BufferedAttackType = EBufferedAttackType::EBAT_None;

		UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EActionState ActionState = EActionState::EAS_Unoccupied;


		UPROPERTY()
		UGameOverlay* GameOverlay;


		UPROPERTY()
		ASpellProjectile* PendingSpellProjectile = nullptr;


		FRotator CastRotation;

		FVector2D MovementInput;


		bool bCanBufferAttack = false;

		float LockOnCameraOffsetYaw = 0.f;
};