// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterAnimInstance.generated.h"


UCLASS()
class SHOOTERGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/* Sets all default values. */
	UShooterAnimInstance();

	/* Performs initialization. */
	virtual void NativeInitializeAnimation() override;

	/* Updates the animation. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/* The owner of the animation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	class AShooterCharacter* CharacterOwner;

	/* The weapon movement animation mode. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	EWeaponAnimationMovementType MovementType;

	/* Is character moving? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bIsMoving : 1;

	/* Is character sprinting? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bIsSprinting : 1;

	/* Is character zooming weapon? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bIsTargeting : 1;

	/* Is character falling?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bIsFalling : 1;

	/* Is character dead?  Based on HealthComponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bIsDead : 1;

	/* Is character crouching?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	uint8 bIsCrouching : 1;

	/* Pitch of character.  Used for aim offsets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Pitch;

	/* Speed of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed;

	/* Direction based on character velocity and rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Direction;
};
