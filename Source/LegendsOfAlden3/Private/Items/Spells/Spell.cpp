#include "Items/Spells/Spell.h"
#include "Items/Spells/SpellProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASpellProjectile* ASpell::BeginCast(ACharacter* Caster) const
{
    if (!Caster || !ProjectileClass) return nullptr;

    const FVector SpawnLocation = Caster->GetMesh()->GetSocketLocation("LeftHandSocket");
    const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

    ASpellProjectile* Projectile = Caster->GetWorld()->SpawnActorDeferred<ASpellProjectile>(ProjectileClass, SpawnTransform, Caster, Caster);

    if (Projectile)
    {
        Projectile->SetDamage(Damage);
        Projectile->ConfigureVFX(TrailEffect, ImpactEffect, ImpactSound);
        Projectile->FinishSpawning(SpawnTransform);
        Projectile->AttachToHand(Caster->GetMesh(), "LeftHandSocket");
    }

    if (CastSound)
    {
        UGameplayStatics::PlaySoundAtLocation(Caster, CastSound, SpawnLocation);
    }

    return Projectile;
}