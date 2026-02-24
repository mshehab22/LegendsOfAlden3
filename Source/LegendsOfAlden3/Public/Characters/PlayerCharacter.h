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

		virtual void Tick(float DeltaTime) override;

		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
		UInputAction* LightAttackAction;

		// Callbacks for input
		void Movement(const FInputActionValue& Value);

		void Look(const FInputActionValue& Value);

		virtual void Jump() override;

		void Interact();

		virtual void LightAttack() override;

		virtual void HeavyAttack() override;

		// Play montage functions
		virtual void PlayAttackMontage(UAnimMontage* MontageToPlay) override;
		virtual void PlayEquipMontage(const FName& SectionName) override;
		virtual void AttackEnd() override;

		virtual bool CanAttack() override;

		virtual bool CanDisarm()override;

		virtual bool CanArm()override;

		virtual bool CanMove()override;

		virtual void FinishEquipping()override;

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
		
		UPROPERTY()
		UAnimMontage* CurrentAttackMontage = nullptr;
		

		bool bAttackBuffered = false;
		bool bCanBufferAttack = false;
		int32 LastAttackIndex = -1;
	
};
