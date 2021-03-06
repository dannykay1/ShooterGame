// Copyright Danny Kay 2020.

#include "Pawns/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/ShooterWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ShooterHealthComponent.h"
#include "Animation/AnimInstance.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UShooterHealthComponent>(TEXT("HealthComp"));

	bIsSprinting = false;
	bIsTargeting = false;

	SprintSpeed = 1200.f;
	WalkSpeed = 600.f;

	CurrentWeaponIndex = -1;
}


// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentWeaponIndex = 0;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < StartingWeapons.Num(); i++)
	{
		AShooterWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AShooterWeapon>(StartingWeapons[i], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (SpawnedWeapon)
		{
			SpawnedWeapon->SetOwner(this);

			// hide all but first weapon.
			if (i > 0)
			{
				SpawnedWeapon->UnequipWeapon();
			}
			else
			{
				SpawnedWeapon->EquipWeapon();
			}

			EquippedWeapons.Add(SpawnedWeapon);
		}
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &AShooterCharacter::OnHealthChanged);
}


void AShooterCharacter::Kill()
{
	for (AShooterWeapon* Weapon : EquippedWeapons)
	{
		if (Weapon)
		{
			Weapon->StopFire();
			Weapon->Destroy();
		}
	}

	DetachFromControllerPendingDestroy();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.f;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	FTimerHandle TimerHandle_SpawnEffects;
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnEffects, this, &AShooterCharacter::PlayDeathEffect, 1.0f);

	SetLifeSpan(10.f);
}


void AShooterCharacter::PlayDeathEffect()
{
	if (DeathEffect && GetMesh())
	{
		GetMesh()->SetHiddenInGame(true);

		FVector SpawnLocation = GetMesh()->GetComponentLocation();
		FRotator SpawnRotation = GetMesh()->GetComponentRotation();

		UNiagaraFunctionLibrary::SpawnSystemAttached(DeathEffect, GetMesh(), NAME_None, SpawnLocation, SpawnRotation, EAttachLocation::SnapToTargetIncludingScale, true);
	}
}


AShooterWeapon* AShooterCharacter::GetCurrentWeapon() const
{
	if (EquippedWeapons.Num() && EquippedWeapons.IsValidIndex(CurrentWeaponIndex))
	{
		return EquippedWeapons[CurrentWeaponIndex];
	}

	return nullptr;
}


EWeaponAnimationMovementType AShooterCharacter::GetWeaponAnimationMovementType() const
{
	if (GetCurrentWeapon())
	{
		return GetCurrentWeapon()->GetAnimationMovementType();
	}

	return EWeaponAnimationMovementType::Pistol;
}


void AShooterCharacter::OnHealthChanged(class UShooterHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		Kill();
	}
}


void AShooterCharacter::StartSprint()
{
	StopFire();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprinting = true;
}


void AShooterCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprinting = false;
}


void AShooterCharacter::StartFire()
{
	if (GetCurrentWeapon())
	{
		StopSprint();
		GetCurrentWeapon()->StartFire();
	}
}


void AShooterCharacter::StopFire()
{
	if (GetCurrentWeapon())
	{
		GetCurrentWeapon()->StopFire();
	}
}


void AShooterCharacter::StartReload()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	UAnimMontage* ReloadMontage = GetCurrentWeapon() ? GetCurrentWeapon()->GetReloadMontage() : nullptr;
	if (ReloadMontage && AnimInstance && !AnimInstance->Montage_IsPlaying(ReloadMontage))
	{
		PlayAnimMontage(ReloadMontage);

		FOnMontageEnded BlendOutDelegate;
		BlendOutDelegate.BindUObject(this, &AShooterCharacter::Reload);

		AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, ReloadMontage);
	}
}


void AShooterCharacter::Reload(class UAnimMontage* MontageToPlay, bool bInterrupted)
{
	if (GetCurrentWeapon() && !bInterrupted)
	{
		GetCurrentWeapon()->ReloadWeapon();
	}
}