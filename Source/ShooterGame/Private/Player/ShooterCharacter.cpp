// Copyright Danny Kay 2020.

#include "Player/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/ShooterWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ShooterHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsSprinting = false;
	SprintSpeed = 1200.f;
	WalkSpeed = 600.f;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SocketOffset = FVector(0.f, 12.f, 125.f);
	SpringArmComp->TargetArmLength = 300.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<UShooterHealthComponent>(TEXT("HealthComp"));

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	BodyColor = FLinearColor(1.f, 1.f, 1.f);
}


// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (StartingWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		EquippedWeapon = GetWorld()->SpawnActor<AShooterWeapon>(StartingWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (EquippedWeapon)
		{
			EquippedWeapon->SetOwner(this);
			EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquippedWeapon->GetWeaponAttachSocketName());
		}
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &AShooterCharacter::OnHealthChanged);

	UMaterialInstanceDynamic* DynamicMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	DynamicMaterial->SetVectorParameterValue("BodyColor", BodyColor);

	DynamicMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(1);
	DynamicMaterial->SetVectorParameterValue("BodyColor", BodyColor);
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterCharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterCharacter::StartSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::StopFire);
}


FVector AShooterCharacter::GetPawnViewLocation() const
{
	if (CameraComp && SpringArmComp)
	{
		FVector ViewLocation = CameraComp->GetComponentLocation();

		// Offset the view location by arm length so character cannot hit actors behind this character.
		ViewLocation += CameraComp->GetForwardVector() * SpringArmComp->TargetArmLength;

		return ViewLocation;
	}

	return Super::GetPawnViewLocation();
}


void AShooterCharacter::PossessedBy(class AController* NewController)
{
	Super::PossessedBy(NewController);

	if (EquippedWeapon)
	{
		EquippedWeapon->SetOwner(this);
	}
}


void AShooterCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}


void AShooterCharacter::DetachFromControllerPendingDestroy()
{
	Super::DetachFromControllerPendingDestroy();
	Kill();
}


void AShooterCharacter::Kill()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.f;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	SetLifeSpan(10.f);
}


EWeaponAnimationMovementType AShooterCharacter::GetWeaponAnimationMovementType() const
{
	if (EquippedWeapon)
	{
		return EquippedWeapon->GetAnimationMovementType();
	}

	return EWeaponAnimationMovementType::Pistol;
}


void AShooterCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);

	if (Value <= 0.f)
	{
		StopSprint();
	}
}


void AShooterCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}


void AShooterCharacter::StartSprint()
{
	StopFire();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprinting = true;
}


void AShooterCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsSprinting = false;
}


void AShooterCharacter::ToggleCrouch()
{
	StopSprint();

	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}


void AShooterCharacter::OnHealthChanged(class UShooterHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{	
		DetachFromControllerPendingDestroy();
	}
}


void AShooterCharacter::StartFire()
{
	if (EquippedWeapon)
	{
		StopSprint();
		EquippedWeapon->StartFire();
	}
}


void AShooterCharacter::StopFire()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}

#if WITH_EDITOR
void AShooterCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UMaterialInstanceDynamic* DynamicMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	DynamicMaterial->SetVectorParameterValue("BodyColor", BodyColor);

	DynamicMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(1);
	DynamicMaterial->SetVectorParameterValue("BodyColor", BodyColor);
}
#endif