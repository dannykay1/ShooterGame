// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterProjectile.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterProjectile();

	void InitVelocity(FVector& ShotDirection, float NewSpeed);
	void SetDamage(float NewDamage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UParticleSystemComponent* ParticleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovementComp;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void PlayImpactEffects(const FRotator ImpactRotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (ExposeOnSpawn = true))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (ExposeOnSpawn = true))
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class USoundCue* ImpactSound;
};
