// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

	void PossessNewPawn();
};
