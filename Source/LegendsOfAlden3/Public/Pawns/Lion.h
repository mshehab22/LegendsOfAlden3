#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Lion.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class LEGENDSOFALDEN3_API ALion : public APawn
{
	GENERATED_BODY()

	public:
		ALion();

		virtual void Tick(float DeltaTime) override;

		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	protected:
		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input) 
		UInputMappingContext* LionMappingContext;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* MoveAction;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* LookAction;

		void Move(const FInputActionValue& Value);

		void Look(const FInputActionValue& Value);

	private:	
		UPROPERTY(VisibleAnywhere) 
		UCapsuleComponent* Capsule;

		UPROPERTY(VisibleAnywhere) 
		USkeletalMeshComponent* LionMesh;

		UPROPERTY(VisibleAnywhere)
		USpringArmComponent* CameraBoom;

		UPROPERTY(VisibleAnywhere)
		UCameraComponent* ViewCamera;
};
