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

protected:
	// Weapon specific fire implementation.
	virtual void FireWeapon() override;
};
