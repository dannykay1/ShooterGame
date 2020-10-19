// Copyright Danny Kay 2020.

#include "Player/ShooterPlayerController.h"
#include "GameFramework/Pawn.h"
#include "ShooterGame/ShooterGame.h"
#include "Components/ShooterHealthComponent.h"


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
		if (Hit.GetActor())
		{
			APawn* HitPawn = Cast<APawn>(Hit.GetActor());

			if (HitPawn)
			{
				UShooterHealthComponent* HealthComp = Cast<UShooterHealthComponent>(HitPawn->GetComponentByClass(UShooterHealthComponent::StaticClass()));

				// Only allow possession of pawn if it has health component and has health (not dead).
				if (HealthComp && HealthComp->GetHealth() > 0.0f)
				{
					// Detach and kill current pawn.
					MyPawn->DetachFromControllerPendingDestroy();

					// Prepare hit pawn by unpossessing from ai controller to be possessed by this controller.
					HitPawn->UnPossessed();

					SetViewTargetWithBlend(HitPawn, 0.5f);
					FTimerDelegate TimerDel_Posses;
					FTimerHandle TimerHandle_Posses;

					TimerDel_Posses.BindUFunction(this, FName("Possess"), HitPawn);
					GetWorldTimerManager().SetTimer(TimerHandle_Posses, TimerDel_Posses, 0.6f, false);
				}
			}
		}
	}
}