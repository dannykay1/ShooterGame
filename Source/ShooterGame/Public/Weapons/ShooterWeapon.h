// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterWeapon.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterWeapon();

	FORCEINLINE EWeaponAnimationMovementType GetAnimationMovementType() const { return AnimationType; }
	FORCEINLINE FName GetWeaponAttachSocketName() const { return WeaponAttachSocketName; }

	void StartFire();
	void StopFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Fire();
	void PlayFireEffects();
	void SpawnProjectile(FVector EndPoint);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	EWeaponAnimationMovementType AnimationType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class USoundCue* MuzzleSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AShooterProjectile> ProjectileClassToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 NumBulletsPerShot;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	/* RPM - Bullets per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	/* Bullet Spread in Degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread;

	// Derived from RateOfFire
	float TimeBetweenShots;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint8 bAutomaticFire : 1;
};
