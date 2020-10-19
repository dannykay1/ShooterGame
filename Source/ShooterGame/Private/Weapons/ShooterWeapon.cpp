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
#include "Player/ShooterCharacter.h"


// Sets default values
AShooterWeapon::AShooterWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MeshComp);

	WeaponAttachSocketName = "Socket_MainWeapon";
	MuzzleSocketName = "Socket_Muzzle";
	AnimationType = EWeaponAnimationMovementType::Pistol;

	NumBulletsPerShot = 1;
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
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AShooterWeapon::Fire, TimeBetweenShots, bAutomaticFire, FirstDelay);
}


void AShooterWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void AShooterWeapon::Fire()
{
	if (OwnerCharacter == nullptr || OwnerCharacter->GetController() == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < NumBulletsPerShot; ++i)
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
		QueryParams.bTraceComplex = true;

		FVector EndPoint = TraceEnd;

		// Hit.
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			EndPoint = Hit.ImpactPoint;
		}

		SpawnProjectile(EndPoint);
	}

	PlayFireEffects();
	LastFireTime = GetWorld()->TimeSeconds;
}


void AShooterWeapon::PlayFireEffects()
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


void AShooterWeapon::SpawnProjectile(FVector EndPoint)
{
	if (ProjectileClassToSpawn && OwnerCharacter)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, EndPoint);

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(MuzzleLocation);
		SpawnTransform.SetRotation(LookRotation.Quaternion());

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerCharacter;
		SpawnParams.Instigator = OwnerCharacter;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AShooterProjectile>(ProjectileClassToSpawn, SpawnTransform, SpawnParams);
	}
}