#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 4000.f;
	SightConfig->LoseSightRadius = 4500.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	
	if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (Hitter)
	{
		CombatTarget = Hitter;
	}

	// If enemy is currently drawing weapon, do not interrupt equip montage
	if (!IsWeaponInHand() && !CanMove())
	{
		if (!IsDead())
		{
			ShowHealthBar();
		}

		return;
	}

	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (!IsDead())
	{
		ShowHealthBar();
	}

	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsEngaged())
	{
		StopAttackMontage();
		EnemyMovement = EEnemyMovement::EEM_CanMove;
	}

	if (!IsDead() && CombatTarget)
	{
		ChaseTarget();
	}
}

void AEnemy::SetEnemyState(EEnemyState NewState)
{
	if (EnemyState == NewState) { return; }

	if (EnemyState == EEnemyState::EES_Dead) { return; }
	
	EnemyState = NewState;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitializeEnemy();

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::OnPerceptionUpdated);
	}

	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	SetEnemyState(EEnemyState::EES_Dead);
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::LightAttack()
{
	Super::LightAttack();
	if (CombatTarget == nullptr) return;

	SetEnemyState(EEnemyState::EES_Engaged);
	EnemyMovement = EEnemyMovement::EEM_CannotMove;

	if (EquippedWeapon && EnemyController)
	{
		EnemyController->StopMovement();
		PlayAttackMontage(EquippedWeapon->GetLightAttackMontage());
	}
}

void AEnemy::HeavyAttack()
{
	Super::HeavyAttack();
	if (CombatTarget == nullptr) return;

	SetEnemyState(EEnemyState::EES_Engaged);
	if (EquippedWeapon && EnemyController)
	{
		EnemyController->StopMovement();
		PlayAttackMontage(EquippedWeapon->GetHeavyAttackMontage());
	}
}

bool AEnemy::CanAttack()
{
	bool bCanAttack = IsWeaponInHand() && IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();

	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	StopAttackMontage();
	SetEnemyState(EEnemyState::EES_NoState);
	EnemyMovement = EEnemyMovement::EEM_CanMove;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::FinishEquipping()
{
	EnemyMovement = EEnemyMovement::EEM_CanMove;
	if (!IsWeaponInHand())
    {
        WeaponType = ECharacterState::ECS_EquippedOneHandedWeapon;
        ChaseTarget();
    }
    else
    {
        WeaponType = ECharacterState::ECS_Unequipped;
        StartPatrolling();
    }
}

int32 AEnemy::PlayAttackMontage(UAnimMontage* Montage)
{
	const int32 Selection = Super::PlayAttackMontage(Montage);
	return Selection;
}

bool AEnemy::CanDisarm()
{
	return (EnemyState >= EEnemyState::EES_Chasing && WeaponType == ECharacterState::ECS_EquippedOneHandedWeapon);
}

bool AEnemy::CanArm()
{
	return (EnemyState == EEnemyState::EES_Patrolling && WeaponType == ECharacterState::ECS_Unequipped);
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	HideHealthBar();
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::EquipWeapon()
{
	if (CanArm())
	{
		EnemyMovement = EEnemyMovement::EEM_CannotMove;
		PlayEquipMontage(FName("Equip"));
	}
}

void AEnemy::UnequipWeapon()
{
	if (CanDisarm())
	{
		EnemyMovement = EEnemyMovement::EEM_CannotMove;
		PlayEquipMontage(FName("Unequip"));
	}
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsWeaponInHand()
{
	return WeaponType != ECharacterState::ECS_Unequipped;
}

bool AEnemy::CanMove()
{
	return 	EnemyMovement == EEnemyMovement::EEM_CanMove;
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius) || !PatrolTarget)
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartPatrolling()
{
	if (IsWeaponInHand())
	{
		if (CanMove())
		{
			UnequipWeapon();
		}

		return;
	}

	if (CanMove())
	{
		SetEnemyState(EEnemyState::EES_Patrolling);
		CheckPatrolTarget();
		GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
		MoveToTarget(PatrolTarget);
	}

}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::ChaseTarget()
{
	if (!IsWeaponInHand())
	{
		if (CanMove())
		{
			EquipWeapon();
		}

		return;
	}

	if (CanMove())
	{
		SetEnemyState(EEnemyState::EES_Chasing);
		GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
		MoveToTarget(CombatTarget);
	}
}

void AEnemy::StartAttackTimer()
{
	SetEnemyState(EEnemyState::EES_Attacking);

	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);

	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::LightAttack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void AEnemy::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* SeenPawn = Cast<APawn>(Actor);
	if (!SeenPawn || !Stimulus.WasSuccessfullySensed()) return;

	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}