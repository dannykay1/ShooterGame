// Copyright Danny Kay 2020.

#include "World/Pickup.h"
#include "Components/SphereComponent.h"
#include "Pawns/ShooterCharacter.h"


// Sets default values
APickup::APickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("Pickup"));
	SetRootComponent(CollisionComp);
}


// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlapBegin);
}


void APickup::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || !OtherActor->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
	if (Player)
	{
		OnPickedUp(Player);
		Destroy();
	}
}
