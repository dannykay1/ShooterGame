// Copyright Danny Kay 2020.

#pragma once

#include "CoreMinimal.h"

#define COLLISION_WEAPON			ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_PROJECTILE		ECollisionChannel::ECC_GameTraceChannel2
#define COLLISION_PICKUP			ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_INTERACTABLE		ECollisionChannel::ECC_GameTraceChannel5

UENUM(BlueprintType)
enum class EWeaponAnimationMovementType : uint8
{
	Pistol,
	Hip,
	Ironsights
};