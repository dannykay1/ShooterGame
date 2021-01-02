// Copyright Danny Kay 2020.

#include "Weapons/ShooterWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundCue.h"
#include "Weapons/ShooterProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Pawns/ShooterCharacter.h"
#include "Animation/AnimMontage.h"


// Sets default values
AShooterWeapon::AShooterWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MeshComp);

	WeaponAttachSocketName = "Socket_MainWeapon";
	HolsterAttachSocketName = "Socket_HolsterMainWeapon";
	MuzzleSocketName = "Socket_Muzzle";

	AnimationType = EWeaponAnimationMovementType::Pistol;

	BaseDamage = 20.f;

	BurstCounter = 1;
	BurstDelay = 0.f;

	BulletSpread = 2.f;
	ZoomedBulletSpread = 1.f;
	RateOfFire = 600;

	bAutomaticFire = false;
}


void AShooterWeapon::SetOwner(class AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	OwnerCharacter = Cast<AShooterCharacter>(NewOwner);

	if (AmmoConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = AmmoConfig.AmmoPerClip;
		CurrentAmmo = AmmoConfig.AmmoPerClip * AmmoConfig.InitialClips;
	}
}


// Called when the game starts or when spawned
void AShooterWeapon::BeginPlay()
{
	Super::BeginPlay();

	LastFireTime = -100.f;
	TimeBetweenShots = 60 / RateOfFire;
}


void AShooterWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AShooterWeapon::ProcessFire, TimeBetweenShots, bAutomaticFire, FirstDelay);
}


void AShooterWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void AShooterWeapon::ProcessFire()
{
	if (OwnerCharacter == nullptr || OwnerCharacter->GetController() == nullptr)
	{
		return;
	}

	if (!HasEnoughAmmo())
	{
		StopFire();
		OwnerCharacter->StartReload();
		return;
	}

	if (BurstCounter > 1 && BurstDelay > 0.f)
	{
		for (int32 i = 0; i < BurstCounter; i++)
		{
			FTimerHandle TimerHande_Burst;
			float Delay = i == 0 ? 0.05f : BurstDelay * i;
			GetWorldTimerManager().SetTimer(TimerHande_Burst, this, &AShooterWeapon::HandleBurstFire, Delay);
		}
	}
	else
	{
		SimulateWeaponFire();
		UseAmmo();

		for (int32 i = 1; i <= BurstCounter; ++i)
		{
			FireWeapon();
		}
	}

	LastFireTime = GetWorld()->TimeSeconds;
}


void AShooterWeapon::HandleBurstFire()
{
	SimulateWeaponFire();
	UseAmmo();
	FireWeapon();
}


FHitResult AShooterWeapon::WeaponTrace()
{
	FVector EyeLocation;
	FRotator EyeRotation;

	OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	// Spread.
	float TargetSpread = OwnerCharacter->GetIsTargeting() ? ZoomedBulletSpread : BulletSpread;

	float HalfRad = FMath::DegreesToRadians(TargetSpread);
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000.f);

	// Collision query params.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams);

	return Hit;
}


FVector AShooterWeapon::GetMuzzleLocation() const
{
	return MeshComp->GetSocketLocation(MuzzleSocketName);
}


void AShooterWeapon::SimulateWeaponFire()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (MuzzleSound)
	{
		if (OwnerCharacter && OwnerCharacter->IsPlayerControlled())
		{
			UGameplayStatics::PlaySound2D(OwnerCharacter, MuzzleSound, 0.5f);
		}
		else
		{
			UGameplayStatics::SpawnSoundAttached(MuzzleSound, MeshComp, MuzzleSocketName);
		}
	}

	if (OwnerCharacter)
	{
		UAnimMontage* TargetMontage = OwnerCharacter->GetIsTargeting() ? FireMontageAiming : FireMontage;
		if (TargetMontage)
		{
			OwnerCharacter->PlayAnimMontage(TargetMontage);
		}
	}
}


void AShooterWeapon::EquipWeapon()
{
	if (OwnerCharacter)
	{
		AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}


void AShooterWeapon::UnequipWeapon()
{
	if (OwnerCharacter)
	{
		AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HolsterAttachSocketName);
	}
}


bool AShooterWeapon::HasEnoughAmmo()
{
	return CurrentAmmoInClip > 0;
}


void AShooterWeapon::UseAmmo()
{
	CurrentAmmoInClip = FMath::Clamp(CurrentAmmoInClip - 1, 0, AmmoConfig.AmmoPerClip);
}


void AShooterWeapon::GiveAmmo(int32 Value)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Value, 0, AmmoConfig.MaxAmmo);
}


void AShooterWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(AmmoConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	CurrentAmmoInClip = FMath::Clamp(CurrentAmmoInClip + ClipDelta, 0, AmmoConfig.AmmoPerClip);

	CurrentAmmo = FMath::Clamp(CurrentAmmo - ClipDelta, 0, AmmoConfig.MaxAmmo);
}