#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

//UENUM(BlueprintType)
//enum class ECharacterState : uint8
//{
//	Locomotion UMETA(DisplayName = "Locomotion"),
//	Jump       UMETA(DisplayName = "Jump"),
//	Attack     UMETA(DisplayName = "Attack"),
//	Dodge      UMETA(DisplayName = "Dodge"),
//	Crouch     UMETA(DisplayName = "Crouch"),
//	Interact   UMETA(DisplayName = "Interact")
//};

UCLASS()
class LEGENDSOFALDEN3_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

	public:
		ACharacterBase();
		virtual void Tick(float DeltaTime) override;
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	protected:
		virtual void BeginPlay() override;

		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		//ECharacterState CharacterState = ECharacterState::Locomotion;

	private:	


};
