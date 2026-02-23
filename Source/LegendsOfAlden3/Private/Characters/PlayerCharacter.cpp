#include "Characters/PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerCharacterMappingContext, 0);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::LightAttack);
	}
}

void APlayerCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && (EquippedWeapon->GetWeaponBox()))
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	if (!CanMove())
	{
		return;
	}

	const FVector2D Movement = Value.Get<FVector2D>();

	if (!Movement.IsNearlyZero())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (CurrentAttackMontage && AnimInstance->Montage_IsPlaying(CurrentAttackMontage))
			{
				AnimInstance->Montage_Stop(0.15f, CurrentAttackMontage); // small blend-out
			}
		}
	}

	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
	const FRotationMatrix RotationMatrix(YawRotation);

	AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::X), Movement.Y);
	AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::Y), Movement.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);

		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APlayerCharacter::Jump()
{
	if (ActionState != EActionState::EAS_Unoccupied) { return; }

	Super::Jump();
}

void APlayerCharacter::Interact()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquippedWeapon = OverlappingWeapon;
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

		CharacterState = 
		EquippedWeapon->GetGripType() == EWeaponGripType::EWGT_TwoHanded ?
		ECharacterState::ECS_EquippedTwoHandedWeapon :
		ECharacterState::ECS_EquippedOneHandedWeapon;

		OverlappingItem = nullptr;
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = EquippedWeapon->GetGripType() ==
				EWeaponGripType::EWGT_TwoHanded ? 
			    ECharacterState::ECS_EquippedTwoHandedWeapon : 
				ECharacterState::ECS_EquippedOneHandedWeapon;

			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

bool APlayerCharacter::CanDisarm()
{
	return (ActionState == EActionState::EAS_Unoccupied) && (CharacterState != ECharacterState::ECS_Unequipped);
}

bool APlayerCharacter::CanArm()
{
	return (ActionState == EActionState::EAS_Unoccupied) && (CharacterState == ECharacterState::ECS_Unequipped) && EquippedWeapon;
}

bool APlayerCharacter::CanMove()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void APlayerCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("WeaponSocket"));
	}
}

void APlayerCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::EnableAttackBuffer()
{
	bCanBufferAttack = true;
}

void APlayerCharacter::LightAttack()
{
	if (CanAttack() && EquippedWeapon)
	{
		PlayAttackMontage(EquippedWeapon->GetLightAttackMontage());
		ActionState = EActionState::EAS_Attacking;
	}
	else if ((ActionState == EActionState::EAS_Attacking) && bCanBufferAttack)
	{
		BufferedAttackType = EBufferedAttackType::EBAT_LightAttack;
	}
}

void APlayerCharacter::HeavyAttack()
{
	if (CanAttack() && EquippedWeapon)
	{
		PlayAttackMontage(EquippedWeapon->GetHeavyAttackMontage());
		ActionState = EActionState::EAS_Attacking;
	}
	else if ((ActionState == EActionState::EAS_Attacking) && bCanBufferAttack)
	{
		BufferedAttackType = EBufferedAttackType::EBAT_HeavyAttack;
	}
}

bool APlayerCharacter::CanAttack()
{
	return (ActionState == EActionState::EAS_Unoccupied) && (EquippedWeapon != nullptr);
}


void APlayerCharacter::PlayAttackMontage(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !MontageToPlay) return;

	CurrentAttackMontage = MontageToPlay;

	AnimInstance->Montage_Play(MontageToPlay);

	int32 NumberOfSections = MontageToPlay->CompositeSections.Num();
	int32 Selection;

	if (NumberOfSections <= 1) { Selection = 1; }
	else
	{
		do
		{
			Selection = FMath::RandRange(1, NumberOfSections);

		}while (Selection == LastAttackIndex);
	}

	LastAttackIndex = Selection;

	FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), Selection));

	AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);

	bCanBufferAttack = false;
	bAttackBuffered = false;
}

void APlayerCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void APlayerCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;

	if (BufferedAttackType != EBufferedAttackType::EBAT_None)
	{
		EBufferedAttackType TypeToExecute = BufferedAttackType;
		BufferedAttackType = EBufferedAttackType::EBAT_None;

		if (TypeToExecute == EBufferedAttackType::EBAT_LightAttack)
		{
			LightAttack();
		}
		else if (TypeToExecute == EBufferedAttackType::EBAT_HeavyAttack)
		{
			HeavyAttack();
		}
	}
}

