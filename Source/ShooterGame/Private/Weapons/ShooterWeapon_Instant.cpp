// Copyright Danny Kay 2020.

#include "Weapons/ShooterWeapon_Instant.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


AShooterWeapon_Instant::AShooterWeapon_Instant()
{
	TrailTargetParam = "TraceEnd";
	BaseDamage = 20.f;
}


void AShooterWeapon_Instant::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Firing instant weapon"));

	FHitResult Hit = WeaponTrace();

	FVector EndPoint = Hit.TraceEnd;

	if (Hit.GetActor())
	{
		EndPoint = Hit.ImpactPoint;

		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), BaseDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, UDamageType::StaticClass());

		PlayImpactEffect(EndPoint, Hit.ImpactNormal.Rotation());
	}

	SpawnTrailEffect(EndPoint);
}


void AShooterWeapon_Instant::PlayImpactEffect(const FVector& EndPoint, const FRotator& Rotation)
{
	if (ImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, EndPoint);
	}

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, EndPoint, Rotation);
	}
}


void AShooterWeapon_Instant::SpawnTrailEffect(const FVector& EndPoint)
{
	if (TrailEffect)
	{
		const FVector Origin = GetMuzzleLocation();

		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailEffect, Origin);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
		}
	}
}