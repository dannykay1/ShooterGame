// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeapon.h"
#include "ShooterWeapon_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterWeapon_Projectile : public AShooterWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShooterWeapon_Projectile();

protected:
	// Weapon specific fire implementation.
	virtual void FireWeapon() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AShooterProjectile> ProjectileClassToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float MuzzleSpeed;
};
