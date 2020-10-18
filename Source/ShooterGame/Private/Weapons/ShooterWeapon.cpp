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
#include "GameFramework/Character.h"


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
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < NumBulletsPerShot; ++i)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Spread.
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000.f);

		// Collision query params.
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
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

	ACharacter* MyCharacter = Cast<ACharacter>(GetOwner());
	if (MyCharacter)
	{
		MyCharacter->PlayAnimMontage(FireMontage);
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}


void AShooterWeapon::SpawnProjectile(FVector EndPoint)
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (ProjectileClassToSpawn && MyOwner)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, EndPoint);

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(MuzzleLocation);
		SpawnTransform.SetRotation(LookRotation.Quaternion());

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = MyOwner;
		SpawnParams.Instigator = MyOwner;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AShooterProjectile>(ProjectileClassToSpawn, SpawnTransform, SpawnParams);
	}
}