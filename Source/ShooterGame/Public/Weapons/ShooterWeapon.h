// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterWeapon.generated.h"

UCLASS(Abstract, Blueprintable)
class SHOOTERGAME_API AShooterWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterWeapon();

	virtual void SetOwner(class AActor* NewOwner);

	FORCEINLINE EWeaponAnimationMovementType GetAnimationMovementType() const { return AnimationType; }
	FORCEINLINE FName GetWeaponAttachSocketName() const { return WeaponAttachSocketName; }

	void StartFire();
	void StopFire();

	void EquipWeapon();
	void UnequipWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ProcessFire();
	void HandleBurstFire();
	virtual void FireWeapon() PURE_VIRTUAL(AShooterWeapon::FireWeapon,);

	FVector GetMuzzleLocation() const;
	void SimulateWeaponFire();

	struct FHitResult WeaponTrace();

	class AShooterCharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName HolsterAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	EWeaponAnimationMovementType AnimationType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* FireMontageAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class USoundCue* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AShooterProjectile> ProjectileClassToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 BurstCounter;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BurstDelay;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	/* RPM - Bullets per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	/* Bullet Spread in Degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread;

	/* Zoomed Bullet Spread in Degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float ZoomedBulletSpread;

	// Derived from RateOfFire
	float TimeBetweenShots;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint8 bAutomaticFire : 1;
};
