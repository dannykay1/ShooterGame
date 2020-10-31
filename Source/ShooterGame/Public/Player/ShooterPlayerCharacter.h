// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/ShooterCharacter.h"
#include "ShooterPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterPlayerCharacter : public AShooterCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Gets the camera as the pawn view location.
	virtual FVector GetPawnViewLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void ToggleCrouch();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Shooter Character")
	void Zoom();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Shooter Character")
	void ToggleShoulder();

	void NextWeapon();
	void PreviousWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;
};
