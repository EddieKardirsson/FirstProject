// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Containers/Array.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initiate CameraBoom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;	// Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true;  // Rotate arm based on the controller

	// Set size for capsule collider
	GetCapsuleComponent()->SetCapsuleSize(50.f, 105.f);

	// Initiate FollowCamera (the camera component for the third person camera)
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	/* Attach the camera to the end of the boom, let the boom adjust to match
       the controller orientation */
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// don't rotate when controller rotates.
    // Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;	//Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ...at this rotation rate.
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.3f;	

	MaxHealth = 100.f;
	Health = 100.f;
	MaxMana = 100.f;
	Mana = 100.f;
	MaxStamina = 100.f;
	Stamina = 100.f;
	Coins = 0.f;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	bShiftKeyDown = false;
	bInteractOn = false;
	bAttacking = false;
	bPrimaryAttacking = false;
	bSecondaryAttacking = false;

	//Init enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
	ManaStatus = EManaStatus::EMS_Normal;

	StaminaDrainRate = 10.f;
	StaminaRecoverRate = 5.f;
	MinSprintStamina = 20.f;
	ManaRegenRate = 5.f;
	MinMana = 10.f;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation() + FVector(0.f, 0.f, 75.f), 25.f, 12, FLinearColor::Green, 10.f, .5);
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStaminaDrain = StaminaDrainRate * DeltaTime;
	float DeltaStaminaRec = StaminaRecoverRate * DeltaTime;
	float DeltaManaRegen = ManaRegenRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if(bShiftKeyDown)
		{
			if (Stamina - DeltaStaminaDrain <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStaminaDrain;
			}
			else Stamina -= DeltaStaminaDrain;
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else //ShiftKey up
		{
			if (Stamina + DeltaStaminaRec >= MaxStamina)
				Stamina = MaxStamina;
			else Stamina += DeltaStaminaRec;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if(bShiftKeyDown)
		{
			if (Stamina - DeltaStaminaDrain <= 0.f) {
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else 
			{
				Stamina -= DeltaStaminaDrain;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else // ShiftKey up
		{
			if(Stamina + DeltaStaminaRec >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStaminaRec;
			}
			else Stamina += DeltaStaminaRec;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else // ShiftKey up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStaminaRec;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStaminaRec >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStaminaRec;
		}
		else Stamina += DeltaStaminaRec;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}

	//switch (ManaStatus)
	//{
	//case EManaStatus::EMS_Normal:
	//	if (Mana + DeltaManaRegen >= MaxMana)
	//		Mana = MaxMana;
	//	else Mana = DeltaManaRegen;
	//	break;
	//case EManaStatus::EMS_BelowMinimum:

	//	break;
	//case EManaStatus::EMS_Exhausted:

	//	break;
	//case EManaStatus::EMS_ExhaustedRecovering:

	//	break;
	//default:
	//	;
	//}
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// third argument calls the address of a function from the parent class ACharacter
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMain::InteractOn);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMain::InteractOff);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AMain::PrimaryAttackOn);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Released, this, &AMain::PrimaryAttackOff);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	// third argument calls the address of a function from the grandparent class APawn
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);	
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);
}

void AMain::MoveForward(float Value) {
	// Checks if the Controller is NOT a nullpointer and if input Value is NOT zero
	if ((Controller != nullptr) && (Value != 0.f) && (!bAttacking)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value) {
	// Checks if the Controller is NOT a nullpointer and if input Value is NOT zero
	if ((Controller != nullptr) && (Value != 0.f) && (!bAttacking)) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::TurnAtRate(float Rate) {

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate) {
	
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::InteractOn()
{
	bInteractOn = true;
	if(ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
}

void AMain::InteractOff()
{
	bInteractOn = false;	
}

void AMain::PrimaryAttackOn()
{
	bPrimaryAttacking = true;
	if (EquippedWeapon)
	{
		LightAttack();		
	}
}

void AMain::PrimaryAttackOff()
{
	bPrimaryAttacking = false;
}



void AMain::DecrementHealth(float A)
{
	if (Health - A <= 0.f) { Health -= A; Die(); }
	else Health -= A;
}

void AMain::Die()
{
	// TO DO
}	

void AMain::IncrementCoins(int32 N) { Coins += N; }

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void AMain::ShiftKeyDown() { bShiftKeyDown = true; }

void AMain::ShiftKeyUp() { bShiftKeyDown = false; }

void AMain::ShowPickupLocations()
{
	/*for(int32 i = 0; i < PickupLocations.Num(); ++i)
		UKismetSystemLibrary::DrawDebugSphere(this,PickupLocations[i], 25.f, 8, FLinearColor::Yellow, 10.f, .5);*/

	for(auto Location : PickupLocations)
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Yellow, 10.f, .5);
}

void AMain::LightAttack()
{
	if(!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.85f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			default:
				;
			}			
		}
	}
}

void AMain::LightAttackEnd()
{
	bAttacking = false;
	if (bPrimaryAttacking)
		LightAttack();
}
