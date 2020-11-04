// Copyright Danny Kay 2020.

#include "Weapons/ShooterWeapon_Projectile.h"
#include "Weapons/ShooterProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Pawns/ShooterCharacter.h"


AShooterWeapon_Projectile::AShooterWeapon_Projectile()
{
	MuzzleSpeed = 3000.0f;
}


void AShooterWeapon_Projectile::FireWeapon()
{
	FHitResult Hit = WeaponTrace();

	FVector EyeLocation;
	FRotator EyeRotation;

	OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	FTransform SpawnTransform(EyeRotation, GetMuzzleLocation());
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClassToSpawn, SpawnTransform));
	if (Projectile)
	{
		Projectile->SetInstigator(GetInstigator());
		Projectile->SetOwner(this);
		Projectile->InitVelocity(ShotDirection, MuzzleSpeed);
		Projectile->SetDamage(BaseDamage);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
	}
}