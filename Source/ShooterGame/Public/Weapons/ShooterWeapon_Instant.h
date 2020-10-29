// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeapon.h"
#include "ShooterWeapon_Instant.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API AShooterWeapon_Instant : public AShooterWeapon
{
	GENERATED_BODY()

public:
	AShooterWeapon_Instant();

protected:
	// Weapon specific fire implementation.
	virtual void FireWeapon() override;

	void PlayImpactEffect(const FVector& EndPoint, const FRotator& Rotation);
	void SpawnTrailEffect(const FVector& EndPoint);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TrailTargetParam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* TrailEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class USoundCue* ImpactSound;
};
