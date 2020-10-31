// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void Kill();

	FORCEINLINE uint8 GetIsSprinting() const { return bIsSprinting; }
	FORCEINLINE uint8 GetIsTargeting() const { return bIsTargeting; }
	
	class AShooterWeapon* GetCurrentWeapon() const;

	// Gets the animation movement type.  Used in animation blueprint.
	EWeaponAnimationMovementType GetWeaponAnimationMovementType() const;

	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void Reload();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

	void StartSprint();
	void StopSprint();

	UFUNCTION()
	void OnHealthChanged(class UShooterHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UShooterHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<class AShooterWeapon>> StartingWeapons;

	TArray<class AShooterWeapon*> EquippedWeapons;

	int32 CurrentWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;

	uint8 bIsSprinting : 1;
	uint8 bIsTargeting : 1;
};
