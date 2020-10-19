// Copyright Danny Kay 2020.

#include "Components/ShooterHealthComponent.h"


// Sets default values for this component's properties
UShooterHealthComponent::UShooterHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Health = DefaultHealth = 100;
	bIsDead = false;

	TeamNum = 255;
}


// Called when the game starts
void UShooterHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakePointDamage.AddDynamic(this, &UShooterHealthComponent::HandleTakePointDamage);
		MyOwner->OnTakeRadialDamage.AddDynamic(this, &UShooterHealthComponent::HandleTakeRadialDamage);
	}
}


void UShooterHealthComponent::Heal(float HealAmount)
{
	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);
}


void UShooterHealthComponent::HandleTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead)
		return;

	float HealthDelta = Health - Damage;

	Health = FMath::Clamp(HealthDelta, 0.f, DefaultHealth);

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, HealthDelta, DamageType, InstigatedBy, DamageCauser);
}

void UShooterHealthComponent::HandleTakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead)
		return;

	float HealthDelta = Health - Damage;

	Health = FMath::Clamp(HealthDelta, 0.f, DefaultHealth);

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, HealthDelta, DamageType, InstigatedBy, DamageCauser);
}


bool UShooterHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// Assume Friendly
		return true;
	}

	UShooterHealthComponent* HealthCompA = Cast<UShooterHealthComponent>(ActorA->GetComponentByClass(UShooterHealthComponent::StaticClass()));
	UShooterHealthComponent* HealthCompB = Cast<UShooterHealthComponent>(ActorB->GetComponentByClass(UShooterHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// Assume friendly
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}
