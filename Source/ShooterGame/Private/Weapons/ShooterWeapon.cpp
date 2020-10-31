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

	BurstCounter = 1;
	BurstDelay = 0.f;

	BulletSpread = 2.0f;
	RateOfFire = 600;

	bAutomaticFire = false;
}


void AShooterWeapon::SetOwner(class AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	OwnerCharacter = Cast<AShooterCharacter>(NewOwner);
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

	if (BurstCounter > 1 && BurstDelay > 0.f)
	{
		for (int32 i = 1; i <= BurstCounter; ++i)
		{
			FTimerHandle TimerHande_Burst;
			GetWorldTimerManager().SetTimer(TimerHande_Burst, this, &AShooterWeapon::HandleBurstFire, BurstDelay * i);
		}
	}
	else
	{
		SimulateWeaponFire();

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
	FireWeapon();
}


FHitResult AShooterWeapon::WeaponTrace()
{
	FVector EyeLocation;
	FRotator EyeRotation;

	OwnerCharacter->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	// Spread.
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
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
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, MeshComp, MuzzleSocketName);
	}

	if (OwnerCharacter)
	{
		OwnerCharacter->PlayAnimMontage(FireMontage);
	}

	if (OwnerCharacter)
	{
		APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
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
