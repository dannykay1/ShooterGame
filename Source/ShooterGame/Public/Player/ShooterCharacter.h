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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Gets the camera as the pawn view location.
	virtual FVector GetPawnViewLocation() const override;

	virtual void PossessedBy(class AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void DetachFromControllerPendingDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void Kill();

	FORCEINLINE uint8 GetIsSprinting() const { return bIsSprinting; }

	// Gets the animation movement type.  Used in animation blueprint.
	EWeaponAnimationMovementType GetWeaponAnimationMovementType() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartSprint();
	void StopSprint();

	void ToggleCrouch();

	UFUNCTION()
	void OnHealthChanged(class UShooterHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Shooter Character")
	void StopFire();

	uint8 bIsSprinting : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UShooterHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AShooterWeapon> StartingWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AShooterWeapon* EquippedWeapon;
};
