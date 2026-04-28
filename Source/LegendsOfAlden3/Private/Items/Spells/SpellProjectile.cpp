#include "Items/Spells/SpellProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ASpellProjectile::ASpellProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
    SetRootComponent(CollisionSphere);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    Mesh->SetupAttachment(RootComponent);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    ProjectileMovement->InitialSpeed = 1200.f;
    ProjectileMovement->MaxSpeed = 1200.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->bAutoActivate = false;

    InitialLifeSpan = 5.f;
}

void ASpellProjectile::ConfigureVFX(UNiagaraSystem* InTrail, UNiagaraSystem* InImpact, USoundBase* InImpactSound)
{
    TrailEffect = InTrail;
    ImpactEffect = InImpact;
    ImpactSound = InImpactSound;
}

void ASpellProjectile::AttachToHand(USceneComponent* AttachTarget, FName SocketName)
{
    if (!AttachTarget) return;
    AttachToComponent(AttachTarget, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void ASpellProjectile::Launch(const FRotator& LaunchRotation)
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetActorRotation(LaunchRotation);

    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    ProjectileMovement->Velocity = LaunchRotation.Vector() * ProjectileMovement->InitialSpeed;
    ProjectileMovement->Activate(true);
}

void ASpellProjectile::BeginPlay()
{
    Super::BeginPlay();

    CollisionSphere->OnComponentHit.AddDynamic(this, &ASpellProjectile::OnHit);

    if (TrailEffect)
    {
        TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            TrailEffect,
            RootComponent,
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true   // auto-destroy when projectile dies
        );
    }
}

void ASpellProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == GetOwner()) return;

    UGameplayStatics::ApplyDamage(OtherActor, Damage,
        GetInstigator() ? GetInstigator()->GetController() : nullptr,
        this, UDamageType::StaticClass());

    if (OtherActor->Implements<UHitInterface>())
    {
        IHitInterface::Execute_GetHit(OtherActor, Hit.ImpactPoint, GetOwner());
    }

    if (ImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation()
        );
    }

    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
    }

    Destroy();
}

void ASpellProjectile::SetDamage(float InDamage)
{
    Damage = InDamage;
}