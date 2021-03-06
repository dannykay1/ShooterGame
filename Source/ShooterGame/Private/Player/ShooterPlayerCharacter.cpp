// Copyright Danny Kay 2020.

#include "Player/ShooterPlayerCharacter.h"
#include "Pawns/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/ShooterWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimMontage.h"
#include "ShooterGame/ShooterGame.h"


AShooterPlayerCharacter::AShooterPlayerCharacter()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 85.f));
	SpringArmComp->TargetArmLength = 250.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PICKUP, ECR_Overlap);

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	bIsViewingRightShoulder = true;

	CameraShoulderOffset = 45.f;
	SpringArmDistanceDefault = 175.f;
	SpringArmDistanceTargeting = 35.f;
}


// Called when the game starts or when spawned
void AShooterPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AShooterPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Zoom
	float TargetArmLength = bIsTargeting ? SpringArmDistanceTargeting : SpringArmDistanceDefault;
	float NewArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetArmLength, DeltaTime, 10.f);

	SpringArmComp->TargetArmLength = NewArmLength;

	// Switch shoulder
	FVector SpringArmLocation = SpringArmComp->GetRelativeLocation();

	FVector TargetSpringArmLocation = FVector(SpringArmLocation);
	TargetSpringArmLocation.Y = bIsViewingRightShoulder ? CameraShoulderOffset : -CameraShoulderOffset;

	FVector NewSpringArmLocation = FMath::VInterpTo(SpringArmLocation, TargetSpringArmLocation, DeltaTime, 30.f);
	SpringArmComp->SetRelativeLocation(NewSpringArmLocation);
}


// Called to bind functionality to input
void AShooterPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterPlayerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterPlayerCharacter::StartSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterPlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterPlayerCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterPlayerCharacter::StartReload);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AShooterPlayerCharacter::Zoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AShooterPlayerCharacter::Zoom);

	PlayerInputComponent->BindAction("ToggleShoulder", IE_Pressed, this, &AShooterPlayerCharacter::ToggleShoulder);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AShooterPlayerCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &AShooterPlayerCharacter::PreviousWeapon);
}


FVector AShooterPlayerCharacter::GetPawnViewLocation() const
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


void AShooterPlayerCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);

	if (Value <= 0.f)
	{
		StopSprint();
	}
}


void AShooterPlayerCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}


void AShooterPlayerCharacter::ToggleCrouch()
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


void AShooterPlayerCharacter::Zoom()
{
	StopSprint();

	bIsTargeting = !bIsTargeting;
}


void AShooterPlayerCharacter::ToggleShoulder()
{
	bIsViewingRightShoulder = !bIsViewingRightShoulder;
}


void AShooterPlayerCharacter::NextWeapon()
{
	PlayAnimMontage(EquipMontage);

	if (EquippedWeapons.IsValidIndex(CurrentWeaponIndex))
	{
		EquippedWeapons[CurrentWeaponIndex]->UnequipWeapon();
	}

	int32 index = CurrentWeaponIndex + 1;
	if (index >= EquippedWeapons.Num())
	{
		index = 0;
	}

	CurrentWeaponIndex = index;

	EquippedWeapons[CurrentWeaponIndex]->EquipWeapon();
}


void AShooterPlayerCharacter::PreviousWeapon()
{
	PlayAnimMontage(EquipMontage);

	if (EquippedWeapons.IsValidIndex(CurrentWeaponIndex))
	{
		EquippedWeapons[CurrentWeaponIndex]->UnequipWeapon();
	}

	int32 index = CurrentWeaponIndex - 1;
	if (index < 0)
	{
		index = EquippedWeapons.Num() - 1;
	}

	CurrentWeaponIndex = index;

	EquippedWeapons[CurrentWeaponIndex]->EquipWeapon();
}