#include "Characters/PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"
#include "HUD/GameHUD.h"
#include "HUD/GameOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Components/DecalComponent.h"
#include "Items/Spells/Spell.h"
#include "Items/Spells/SpellProjectile.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = JumpHeight;
	GetCharacterMovement()->GravityScale = GravityScale;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
	ViewCamera->bUsePawnControlRotation = false;

	LockOnDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("LockOnDecal"));
	LockOnDecal->SetupAttachment(RootComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotationState();
	HandleLockOn(DeltaTime);

	if (Attributes)
	{
		Attributes->RegenerateMana(DeltaTime);
	}
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

		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::HeavyAttack);

		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::StartRun);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopRun);

		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Completed, this, &APlayerCharacter::ToggleLockOn);

		EnhancedInputComponent->BindAction(SpellAction, ETriggerEvent::Started, this, &APlayerCharacter::CastSpell);
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	return DamageAmount;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));

	InitializeLockOnDecal();

	if (LockOnMaterial)
	{
		LockOnDecal->SetDecalMaterial(LockOnMaterial);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerCharacterMappingContext, 0);
		}

		InitializeGameOverlay(PlayerController);
	}

	if (Attributes)
	{
		Attributes->OnHealthChanged.AddDynamic(this, &APlayerCharacter::HandleHealthChanged);
		Attributes->OnManaChanged.AddDynamic(this, &APlayerCharacter::HandleManaChanged);
	}
}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}

	if (PendingSpellProjectile)
	{
		PendingSpellProjectile->Destroy();
		PendingSpellProjectile = nullptr;
	}
}

void APlayerCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void APlayerCharacter::AddSouls(ASoul* Soul)
{ 
	if (Attributes)
	{
		Attributes->AddSouls(Soul->GetSouls());
	}
}

void APlayerCharacter::AddGold(ATreasure* Gold)
{
	if (Attributes)
	{
		Attributes->AddGold(Gold->GetGold());
	}
}

void APlayerCharacter::ToggleLockOn()
{
	if (bIsLockedOn)
	{
		ClearLockOn();
		return;
	}

	AActor* Target = FindBestTarget();

	if (Target)
	{
		SetLockOnTarget(Target);
	}
}

void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	if (!CanMove())
	{
		return;
	}

	const FVector2D Movement = Value.Get<FVector2D>();
	MovementInput = Movement;

	if (!Movement.IsNearlyZero())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (CurrentAttackMontage && AnimInstance && AnimInstance->Montage_IsPlaying(CurrentAttackMontage))
			{
				AnimInstance->Montage_Stop(0.2f, CurrentAttackMontage); // small blend-out
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

	if (bIsLockedOn)
	{
		LockOnCameraOffsetYaw += LookAxisValue.X * 2.f;
		LockOnCameraOffsetYaw = FMath::Clamp(LockOnCameraOffsetYaw, -60.f, 60.f);
		return;
	}

	if (ActionState == EActionState::EAS_Attacking) return;

	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);

		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APlayerCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void APlayerCharacter::Interact()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		// Edit: make it that if I pickup another weapon it goes into the inventory and keep the current equipped weapon
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}

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
	if (CanAttack() && EquippedWeapon)
	{
		ActionState = EActionState::EAS_Attacking;
		CurrentAttackMontage = EquippedWeapon->GetLightAttackMontage();
		PlayAttackMontage(CurrentAttackMontage);
	}
	else if ((ActionState == EActionState::EAS_Attacking) && bCanBufferAttack && EquippedWeapon)
	{
		BufferedAttackType = EBufferedAttackType::EBAT_LightAttack;
	}
}

void APlayerCharacter::HeavyAttack()
{
	if (CanAttack() && EquippedWeapon)
	{
		ActionState = EActionState::EAS_Attacking;
		CurrentAttackMontage = EquippedWeapon->GetHeavyAttackMontage();
		PlayAttackMontage(CurrentAttackMontage);
	}
	else if ((ActionState == EActionState::EAS_Attacking) && bCanBufferAttack && EquippedWeapon)
	{
		BufferedAttackType = EBufferedAttackType::EBAT_HeavyAttack;
	}
}

void APlayerCharacter::Dodge()
{
	if (!IsUnoccupied() || !IsGrounded()) return;

	float ForwardValue = MovementInput.Y;
	float RightValue = MovementInput.X;
	const FVector Forward = GetActorForwardVector();
	const FVector Right = GetActorRightVector();
	FVector InputVector = (Forward * ForwardValue) + (Right * RightValue);
	InputVector = InputVector.GetSafeNormal();
	
	const double CosTheta = FMath::Clamp(FVector::DotProduct(Forward, InputVector), -1.f, 1.f);
	double Theta = FMath::RadiansToDegrees(FMath::Acos(CosTheta));

	const FVector CrossProduct = FVector::CrossProduct(Forward, InputVector);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	Theta = FMath::Fmod(Theta + 360.f, 360.f);

	int32 DirectionIndex = FMath::FloorToInt((Theta + 22.5f) / 45.f) % 8;

	static const FName Sections[8] =
	{
		"Forward",
		"ForwardRight",
		"Right",
		"BackwardRight",
		"Backward",
		"BackwardLeft",
		"Left",
		"ForwardLeft"
	};

	FName Section = Sections[DirectionIndex];

	ActionState = EActionState::EAS_Dodge;
	PlayDodgeMontage(Section);
}

void APlayerCharacter::StartRun()
{
	if (!CanMove()) return;

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APlayerCharacter::StopRun()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::CastSpell()
{
	if (!CanCastSpell()) return;

	// Commit to the cast: lock state and deduct cost up front.
	ActionState = EActionState::EAS_UsingAbility;

	CastRotation = GetControlRotation();

	SetActorRotation(FRotator(0.f, CastRotation.Yaw, 0.f));
	if (Attributes && EquippedSpellClass)
	{
		const ASpell* SpellCDO = EquippedSpellClass->GetDefaultObject<ASpell>();
		if (SpellCDO)
		{
			Attributes->ManaUsage(SpellCDO->ManaCost);
		}
	}

	PlayAnimMontage(SpellCastMontage);
}

bool APlayerCharacter::CanCastSpell() const
{
	return EquippedSpellClass && SpellCastMontage && HasEnoughMana() && (ActionState == EActionState::EAS_Unoccupied);
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon) // Edit: make equipping for two handed socket
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

void APlayerCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::CanAttack() // Edit: make it to check if weapon is in hand
{
	return (IsUnoccupied()) && (CharacterState != ECharacterState::ECS_Unequipped);
}

bool APlayerCharacter::CanDisarm()
{
	return (IsUnoccupied()) && (CharacterState != ECharacterState::ECS_Unequipped);
}

bool APlayerCharacter::CanArm()
{
	return (IsUnoccupied()) && (CharacterState == ECharacterState::ECS_Unequipped) && EquippedWeapon;
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
	return IsUnoccupied();
}

void APlayerCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

int32 APlayerCharacter::PlayAttackMontage(UAnimMontage* Montage)
{
	const int32 Selection = Super::PlayAttackMontage(Montage);
	bCanBufferAttack = false;
	return Selection;
}

void APlayerCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;

	DisableMeshCollision();
}

bool APlayerCharacter::HasEnoughMana() const
{
	if (!Attributes || !EquippedSpellClass) return false;

	const ASpell* SpellDefaults = EquippedSpellClass->GetDefaultObject<ASpell>();
	if (!SpellDefaults) return false;

	return Attributes->GetMana() >= SpellDefaults->ManaCost;
}

void APlayerCharacter::EnableAttackBuffer()
{
	bCanBufferAttack = true;
}

void APlayerCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied; 
}

void APlayerCharacter::SpawnSpell()
{
	if (!EquippedSpellClass) return;

	const ASpell* SpellCDO = EquippedSpellClass->GetDefaultObject<ASpell>();
	if (!SpellCDO) return;

	PendingSpellProjectile = SpellCDO->BeginCast(this);
}

void APlayerCharacter::LaunchSpell()
{
	if (!PendingSpellProjectile) return;

	PendingSpellProjectile->Launch(CastRotation);
	PendingSpellProjectile = nullptr;
}

void APlayerCharacter::CastEnd()
{
	if (ActionState == EActionState::EAS_UsingAbility)
	{
		ActionState = EActionState::EAS_Unoccupied;
	}
}

void APlayerCharacter::HandleLockOn(float DeltaTime)
{
	if (!bIsLockedOn) return;

	if (!IsValid(LockedTarget))
    {
        ClearLockOn();
        return;
    }

	LockOnCameraOffsetYaw = FMath::FInterpTo(LockOnCameraOffsetYaw, 0.f, DeltaTime, 1.5f);

	FVector PlayerLocation = GetActorLocation();
	FVector TargetLocation = LockedTarget->GetActorLocation();

	float Distance = FVector::Dist(PlayerLocation, TargetLocation);
	if (Distance > LockOnRadius)
	{
		ClearLockOn();
		return;
	}

	// Rotate player
	FVector Direction = TargetLocation - PlayerLocation;
	FRotator TargetRotation = Direction.Rotation();

	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.f);

	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));

	// Rotate camera
	if (AController* PC = GetController())
	{
		FVector MidPoint = PlayerLocation + (TargetLocation - PlayerLocation) * 0.6f;

		FRotator BaseRotation = (MidPoint - ViewCamera->GetComponentLocation()).Rotation();

		BaseRotation.Yaw += LockOnCameraOffsetYaw;

		FRotator Smoothed = FMath::RInterpTo(PC->GetControlRotation(), BaseRotation, DeltaTime, 3.f);

		PC->SetControlRotation(Smoothed);
	}
	UpdateLockOnDecal();
}

void APlayerCharacter::SetLockOnTarget(AActor* Target)
{
	LockedTarget = Target;
	if (LockedTarget)
	{
		LockOnDecal->SetVisibility(true);
	}

	CombatTarget = Target;
	bIsLockedOn = true;
}

void APlayerCharacter::ClearLockOn()
{
	LockOnDecal->SetVisibility(false);
	LockedTarget = nullptr;
	CombatTarget = nullptr;
	bIsLockedOn = false;
}

AActor* APlayerCharacter::FindBestTarget()
{
	TArray<AActor*> OverlappingActors;

	FVector Center = GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	UKismetSystemLibrary::SphereOverlapActors
	(
		GetWorld(),
		Center,
		LockOnRadius,
		ObjectTypes,
		nullptr,
		IgnoreActors,
		OverlappingActors
	);

	AActor* BestTarget = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor->ActorHasTag("Enemy")) continue;

		FVector ToTarget = (Actor->GetActorLocation() - GetActorLocation());
		float Distance = ToTarget.Size();

		FVector ToTargetNormalized = ToTarget.GetSafeNormal();
		FVector CameraForward = GetControlRotation().Vector();
		float Dot = FVector::DotProduct(CameraForward, ToTargetNormalized);
		float AnglePenalty = (1.f - Dot) * 300.f;
		float Score = Distance + AnglePenalty;

		if (Score < BestScore)
		{
			BestScore = Score;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}

bool APlayerCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::IsGrounded()
{
	return !GetCharacterMovement()->IsFalling();
}

void APlayerCharacter::InitializeGameOverlay(APlayerController* PlayerController)
{
	if (AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD()))
	{
		GameOverlay = GameHUD->GetGameOverlay();
		if (GameOverlay && Attributes)
		{
			GameOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			GameOverlay->SetManaBarPercent(Attributes->GetManaPercent());
		}
	}
}

void APlayerCharacter::SetHUDHealth()
{
	if (GameOverlay && Attributes)
	{
		GameOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void APlayerCharacter::UpdateHUD(float DeltaTime)
{
	if (Attributes && GameOverlay)
	{
		Attributes->RegenerateMana(DeltaTime);
		GameOverlay->SetManaBarPercent(Attributes->GetManaPercent());
	}
}

void APlayerCharacter::UpdateRotationState()
{
	float Speed = GetVelocity().Size2D();
	if (Speed < 3.f)
	{
		bUseControllerRotationYaw = false;
	}
	else
	{
		bUseControllerRotationYaw = true;
	}
}

void APlayerCharacter::InitializeLockOnDecal()
{
	if (!LockOnDecal) return;

	LockOnDecal->SetVisibility(false);
	LockOnDecal->DecalSize = FVector(70.f, 70.f, 70.f);

	if (LockOnMaterial)
	{
		LockOnDecal->SetDecalMaterial(LockOnMaterial);
	}
}

void APlayerCharacter::UpdateLockOnDecal()
{
	if (!LockedTarget) return;

	FVector Location = LockedTarget->GetActorLocation();
	Location.Z -= 90.f; 

	LockOnDecal->SetWorldLocation(Location);

	LockOnDecal->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
}

void APlayerCharacter::HandleHealthChanged(float NewPercent)
{
	if (GameOverlay)
	{
		GameOverlay->SetHealthBarPercent(NewPercent);
	}
}

void APlayerCharacter::HandleManaChanged(float NewPercent)
{
	if (GameOverlay)
	{
		GameOverlay->SetManaBarPercent(NewPercent);
	}
}
