// Copyright Danny Kay 2020.

#include "Player/ShooterPlayerController.h"
#include "GameFramework/Pawn.h"
#include "ShooterGame/ShooterGame.h"


void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Possess", IE_Pressed, this, &AShooterPlayerController::PossessNewPawn);
}


void AShooterPlayerController::PossessNewPawn()
{
	APawn* MyPawn = GetPawn();
	if (MyPawn == nullptr)
	{
		return;
	}

	FVector EyeLocation;
	FRotator EyeRotation;

	MyPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000.f);

	// Collision query params.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyPawn);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(20.0f);

	// Hit.
	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, EyeLocation, TraceEnd, FQuat::Identity, COLLISION_WEAPON, CollisionSphere))
	{
		if (Hit.GetActor()->IsValidLowLevel())
		{
			APawn* HitPawn = Cast<APawn>(Hit.GetActor());
			if (HitPawn)
			{
				MyPawn->UnPossessed();
				Possess(HitPawn);
			}
		}
	}
}