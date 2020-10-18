// Copyright Danny Kay 2020.

#include "Weapons/ShooterProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterGame/ShooterGame.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"


// Sets default values
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	SetRootComponent(SphereComp);

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->SetupAttachment(SphereComp);
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1500.f;
	ProjectileMovementComp->MaxSpeed = 1500.f;
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	Damage = 20.0f;
	bSuccessfulHit = false;
}


// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();	

	SetLifeSpan(30.0f);

	if (SphereComp)
	{
		SphereComp->IgnoreActorWhenMoving(GetOwner(), true);
		SphereComp->OnComponentHit.AddDynamic(this, &AShooterProjectile::OnProjectileHit);
	}
}


void AShooterProjectile::OnProjectileHit(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || bSuccessfulHit)
	{
		return;
	}

	bSuccessfulHit = true;

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), Hit.ImpactNormal.Rotation());
	}

	if (ImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}

	OnSuccessfulHit(HitComponent, OtherActor,OtherComp, NormalImpulse, Hit);

	UGameplayStatics::ApplyPointDamage(OtherActor, Damage, Hit.TraceStart, Hit, GetOwner()->GetInstigatorController(), GetOwner(), DamageType);

	ParticleComp->Deactivate();

	SetLifeSpan(1.0f);
}