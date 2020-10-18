// Copyright Danny Kay 2020.

#include "Player/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/ShooterWeapon.h"
#include "Kismet/KismetMathLibrary.h"


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

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
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

	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AShooterCharacter::StartSprint);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::StopFire);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AShooterCharacter::Interact);
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


void AShooterCharacter::Interact()
{
	FVector EyeLocation;
	FRotator EyeRotation;

	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000.f);

	// Collision query params.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(EquippedWeapon);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	// Hit.
	FHitResult Hit;
	/*if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		if (Hit.GetActor()->IsValidLowLevel())
		{
			AShooterCharacter* HitCharacter = Cast<AShooterCharacter>(Hit.GetActor());
			if (HitCharacter)
			{
				GetController()->Possess(HitCharacter);
			}
		}
	}*/
}