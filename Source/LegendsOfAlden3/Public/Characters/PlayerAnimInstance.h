#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "PlayerAnimInstance.generated.h"

class APlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class LEGENDSOFALDEN3_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:
		virtual void NativeInitializeAnimation() override;
		virtual void NativeUpdateAnimation(float DeltaTime) override;

		UPROPERTY(BlueprintReadOnly)
		APlayerCharacter* PlayerCharacter;

		UPROPERTY(BlueprintReadOnly, Category = Movement)
		UCharacterMovementComponent* PlayerCharacterMovement;

		UPROPERTY(BlueprintReadOnly, Category = Movement)
		float GroundSpeed;

		UPROPERTY(BlueprintReadOnly, Category = Movement)
		bool IsFalling;

		UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
		ECharacterState CharacterState;

	protected:

	private:
	
};
