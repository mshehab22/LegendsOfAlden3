#include "Characters/PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
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

		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::LightAttack);
	}
}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionState = EActionState::EAS_HitReaction;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerCharacterMappingContext, 0);
		}
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
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void APlayerCharacter::LightAttack()
{
	Super::LightAttack();

	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
	}
	else if ((ActionState == EActionState::EAS_Attacking) && bCanBufferAttack)
	{
		BufferedAttackType = EBufferedAttackType::EBAT_LightAttack;
	}
}

void APlayerCharacter::HeavyAttack()
{
	Super::HeavyAttack();

	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;
	}
	else if ((ActionState == EActionState::EAS_Attacking) && bCanBufferAttack)
	{
		BufferedAttackType = EBufferedAttackType::EBAT_HeavyAttack;
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	EquippedWeapon = Weapon;
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState =
		EquippedWeapon->GetGripType() == EWeaponGripType::EWGT_TwoHanded ?
		ECharacterState::ECS_EquippedTwoHandedWeapon :
		ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
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

bool APlayerCharacter::CanAttack()
{
	return (ActionState == EActionState::EAS_Unoccupied) && (EquippedWeapon != nullptr);
}

bool APlayerCharacter::CanDisarm()
{
	return (ActionState == EActionState::EAS_Unoccupied) && (CharacterState != ECharacterState::ECS_Unequipped);
}

bool APlayerCharacter::CanArm()
{
	return (ActionState == EActionState::EAS_Unoccupied) && (CharacterState == ECharacterState::ECS_Unequipped) && EquippedWeapon;
}

void APlayerCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = EquippedWeapon->GetGripType() ==
		EWeaponGripType::EWGT_TwoHanded ?
		ECharacterState::ECS_EquippedTwoHandedWeapon :
		ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool APlayerCharacter::CanMove()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void APlayerCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

int32 APlayerCharacter::PlayAttackMontage(UAnimMontage* Montage)
{
	const int32 Selection = Super::PlayAttackMontage(Montage);
	bCanBufferAttack = false;
	bAttackBuffered = false;
	return Selection;
}

void APlayerCharacter::EnableAttackBuffer()
{
	bCanBufferAttack = true;
}

void APlayerCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied; 
}

