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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UParticleSystemComponent* ParticleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovementComp;	

	UFUNCTION()
	void OnProjectileHit(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Projectile")
	void OnSuccessfulHit(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	class USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<class UDamageType> DamageType;

	float Damage;
	uint8 bSuccessfulHit : 1;
};
