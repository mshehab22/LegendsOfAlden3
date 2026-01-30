#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AldenAnimations.generated.h"

class AAlden;
class UCharacterMovementComponent;

UCLASS()
class LEGENDSOFALDEN3_API UAldenAnimations : public UAnimInstance
{
	GENERATED_BODY()
	
	public:
		virtual void NativeInitializeAnimation() override;
		virtual void NativeUpdateAnimation(float DeltaTime) override;

		UPROPERTY(BlueprintReadOnly)
		AAlden* Alden;

		UPROPERTY(BlueprintReadOnly, Category = Movement)
		UCharacterMovementComponent* AldenMovement;

		UPROPERTY(BlueprintReadOnly, Category = Movement)
		float GroundSpeed;

		UPROPERTY(BlueprintReadOnly, Category = Movement)
		bool IsFalling;

	protected:

	private:
};
