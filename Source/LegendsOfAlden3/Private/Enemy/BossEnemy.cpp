#include "Enemy/BossEnemy.h"
#include "AIController.h"
#include "HUD/GameHUD.h"
#include "HUD/GameOverlay.h"
#include "GameFramework/PlayerController.h"
#include "Components/AttributeComponent.h"

ABossEnemy::ABossEnemy()
{
    // Boss moves slower but hits harder
    // PatrollingSpeed and ChasingSpeed are private in AEnemy
    // Override in the Blueprint instead (see step 3)
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay(); // calls InitializeEnemy() → SpawnDefaultWeapon()
    Tags.Add(FName("Boss"));

    // Boss skips the equip animation dance — move weapon from back to hand manually
    AttachWeaponToHand();
    WeaponType = ECharacterState::ECS_EquippedOneHandedWeapon;

    // Hide boss HUD bar until the fight begins (trigger volume will show it)
    UGameOverlay* Overlay = GetGameOverlay();
    if (Overlay) Overlay->SetBossHealthBarVisible(false);

    if (BossSpellClass)
    {
        BossSpell = GetWorld()->SpawnActor<ASpell>(BossSpellClass);
    }
}

void ABossEnemy::StartAttackTimer()
{
    SetEnemyState(EEnemyState::EES_Attacking);
    const float AttackTime = FMath::RandRange(BossAttackMin, BossAttackMax);
    GetWorldTimerManager().SetTimer(BossAttackTimer, this, &ABossEnemy::DecideAttack, AttackTime);
}

void ABossEnemy::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(BossAttackTimer);
}

UGameOverlay* ABossEnemy::GetGameOverlay() const
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return nullptr;
    AGameHUD* HUD = Cast<AGameHUD>(PC->GetHUD());
    if (!HUD) return nullptr;
    return HUD->GetGameOverlay();
}

void ABossEnemy::HandleDamage(float DamageAmount)
{
    Super::HandleDamage(DamageAmount); // calls AEnemy → ACharacterBase → Attributes->ReceiveDamage

    if (Attributes)
    {
        UGameOverlay* Overlay = GetGameOverlay();
        if (Overlay) Overlay->SetBossHealthBarPercent(Attributes->GetHealthPercent());
    }
}

void ABossEnemy::Die_Implementation()
{
    Super::Die_Implementation(); // handles AI disable, animation, capsule, soul spawn
    OnBossDied.Broadcast();      // level Blueprint listens to this

    UGameOverlay* Overlay = GetGameOverlay();
    if (Overlay) Overlay->SetBossHealthBarVisible(false);
}

void ABossEnemy::SpecialAttack()
{
    if (BossSpell == nullptr || CombatTarget == nullptr) return;
    SetEnemyState(EEnemyState::EES_Engaged);
    EnemyMovement = EEnemyMovement::EEM_CannotMove;

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController) AIController->StopMovement();

    BossSpell->Cast(this);

    // No montage = no AnimNotify, so manually trigger AttackEnd after delay
    GetWorldTimerManager().SetTimer(RecoveryTimer, this, &ABossEnemy::AttackEnd, 1.5f);
}

bool ABossEnemy::CanAttack()
{
    return Super::CanAttack() && GetEnemyState() != EEnemyState::EES_Recovering;
}

void ABossEnemy::AttackEnd()
{
    StopAttackMontage();
    SetEnemyState(EEnemyState::EES_Recovering);
    EnemyMovement = EEnemyMovement::EEM_CanMove;
    GetWorldTimerManager().SetTimer(RecoveryTimer, this, &ABossEnemy::RecoveryFinished, RecoveryTime);
}

void ABossEnemy::RecoveryFinished()
{
    SetEnemyState(EEnemyState::EES_NoState);
    CheckCombatTarget();
}

void ABossEnemy::DecideAttack()
{
    if (CombatTarget == nullptr) return;
    const float Distance = FVector::Dist(GetActorLocation(), CombatTarget->GetActorLocation());
    const int32 Roll = FMath::RandRange(0, 2);

    if (Distance > 400.f)
    {
        SpecialAttack(); // ranged if far
    }
    else if (Roll == 0)
    {
        HeavyAttack();
    }
    else
    {
        LightAttack(); // AEnemy melee
    }
}
