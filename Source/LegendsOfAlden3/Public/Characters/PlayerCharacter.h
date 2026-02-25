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


UCLASS()
class LEGENDSOFALDEN3_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

	public:
		APlayerCharacter();
		
		/** <AActor> */
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
		/** </AActor> */

		FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
		FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	protected:
		virtual void BeginPlay() override;

		// Callbacks for input
		void Movement(const FInputActionValue& Value);
		void Look(const FInputActionValue& Value);
		virtual void Jump() override;
		void Interact();
		virtual void LightAttack() override;
		virtual void HeavyAttack() override;

		/** Combat */
		void EquipWeapon(AWeapon* Weapon);
		virtual void AttackEnd() override;
		virtual bool CanAttack() override;
		virtual bool CanDisarm() override;
		virtual bool CanArm() override;
		virtual bool CanMove() override;
		virtual void FinishEquipping()override;
		virtual int32 PlayAttackMontage(UAnimMontage* Montage) override;

		UFUNCTION(BlueprintCallable)
		void EnableAttackBuffer();

	private:

		/** Input Variables*/
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
		UInputAction* LightAttackAction;
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
		
		bool bAttackBuffered = false;
		bool bCanBufferAttack = false;

		ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

		EBufferedAttackType BufferedAttackType = EBufferedAttackType::EBAT_None;

		UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EActionState ActionState = EActionState::EAS_Unoccupied;

	
};
