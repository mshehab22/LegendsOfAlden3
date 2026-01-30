#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "InputActionValue.h"
#include "Alden.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class LEGENDSOFALDEN3_API AAlden : public ACharacterBase
{
	GENERATED_BODY()

	public:
		AAlden();

		virtual void Tick(float DeltaTime) override;

		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	protected:
		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputMappingContext* AldenMappingContext;
		
		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* MovementAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* LookAction;

		UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* JumpAction;

		void Movement(const FInputActionValue& Value);
		void Look(const FInputActionValue& Value);
		virtual void Jump() override;

	private:
		UPROPERTY(VisibleAnywhere)
		USpringArmComponent* CameraBoom;

		UPROPERTY(VisibleAnywhere)
		UCameraComponent* ViewCamera;
};
