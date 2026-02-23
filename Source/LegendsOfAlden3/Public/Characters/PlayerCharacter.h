#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AWeapon;

UCLASS()
class LEGENDSOFALDEN3_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

	public:
		APlayerCharacter();

		virtual void Tick(float DeltaTime) override;

		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

		FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
		FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	protected:
		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputMappingContext* PlayerCharacterMappingContext;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* MovementAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* LookAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* JumpAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* InteractAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* AttackAction;

		// Callbacks for input
		void Movement(const FInputActionValue& Value);

		void Look(const FInputActionValue& Value);

		virtual void Jump() override;

		void Interact();

		void LightAttack();

		void HeavyAttack();

		// Play montage functions
		void PlayAttackMontage(UAnimMontage* MontageToPlay);
		void PlayEquipMontage(const FName& SectionName);

		UFUNCTION(BlueprintCallable)
		void AttackEnd();

		bool CanAttack();

		bool CanDisarm();

		bool CanArm();

		bool CanMove();

		UFUNCTION(BlueprintCallable)
		void Disarm();

		UFUNCTION(BlueprintCallable)
		void Arm();

		UFUNCTION(BlueprintCallable)
		void FinishEquipping();

		UFUNCTION(BlueprintCallable)
		void EnableAttackBuffer();

	private:
		ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

		EBufferedAttackType BufferedAttackType = EBufferedAttackType::EBAT_None;

		UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EActionState ActionState = EActionState::EAS_Unoccupied;

		UPROPERTY(VisibleAnywhere)
		USpringArmComponent* CameraBoom;

		UPROPERTY(VisibleAnywhere)
		UCameraComponent* ViewCamera;

		UPROPERTY(VisibleInstanceOnly)
		AItem* OverlappingItem;

		UPROPERTY(VisibleAnywhere, Category = Weapon)
		AWeapon* EquippedWeapon;
		
		UPROPERTY()
		UAnimMontage* CurrentAttackMontage = nullptr;
		
		// Animation montages
		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* AttackMontage;

		UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* EquipMontage;

		bool bAttackBuffered = false;
		bool bCanBufferAttack = false;
		int32 LastAttackIndex = -1;
	
};
