#include "Items/Spells/Spell.h"
#include "Items/Spells/SpellProjectile.h"
#include "GameFramework/Character.h"

void ASpell::Cast(ACharacter* Caster) const
{
	if (!Caster || !ProjectileClass) return;

	FVector SpawnLocation = Caster->GetMesh()->GetSocketLocation("LeftHandSocket");
	FRotator SpawnRotation = Caster->GetControlRotation();

	FActorSpawnParameters Params;
	Params.Owner = Caster;

	ASpellProjectile* Projectile = Caster->GetWorld()->SpawnActor<ASpellProjectile>( ProjectileClass, SpawnLocation, SpawnRotation, Params);

	if (Projectile)
	{
		Projectile->SetDamage(Damage);
	}
}