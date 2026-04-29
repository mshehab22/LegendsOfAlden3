#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;

UCLASS()
class LEGENDSOFALDEN3_API ASpellProjectile : public AActor
{
    GENERATED_BODY()

    public:
        ASpellProjectile();

        void SetDamage(float InDamage);

        void ConfigureVFX(UNiagaraSystem* InTrail, UNiagaraSystem* InImpact, USoundBase* InImpactSound);

        void AttachToHand(USceneComponent* AttachTarget, FName SocketName);

        void Launch(const FRotator& LaunchRotation);

    protected:
        virtual void BeginPlay() override;

    private:
        UPROPERTY(VisibleAnywhere)
        USphereComponent* CollisionSphere;

        UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* Mesh;

        UPROPERTY(VisibleAnywhere)
        UProjectileMovementComponent* ProjectileMovement;

        UPROPERTY()
        UNiagaraComponent* TrailComponent;

        UPROPERTY()
        UNiagaraSystem* TrailEffect;

        UPROPERTY()
        UNiagaraSystem* ImpactEffect;

        UPROPERTY()
        USoundBase* ImpactSound;

        float Damage;

        UFUNCTION()
        void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};