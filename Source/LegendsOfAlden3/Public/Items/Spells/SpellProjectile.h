#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class LEGENDSOFALDEN3_API ASpellProjectile : public AActor
{
	GENERATED_BODY()

	public:
		ASpellProjectile();

		void SetDamage(float InDamage);

	protected:
		virtual void BeginPlay() override;

	private:
		UPROPERTY(VisibleAnywhere)
		USphereComponent* CollisionSphere;

		UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

		UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovement;

		float Damage;

		UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};