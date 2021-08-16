// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EManaStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	EMS_Exhausted UMETA(DisplayName = "Exhausted"),
	EMS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	class TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	/** The status of the player's movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	/** The status of the player stamina */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	/** The status of the player mana */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EManaStatus ManaStatus;

	/** Set movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float StaminaRecoverRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float MinSprintStamina;

	FORCEINLINE void SetManaStatus(EManaStatus Status) { ManaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float ManaRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseStats")
	float MinMana;

	/** The speed for the default running speed of the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	float RunningSpeed;

	/** The speed for the SPRINTING speed of the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	float SprintingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BaseStats")
	bool bShiftKeyDown;

	/** Press down shift key to enable sprinting */
	void ShiftKeyDown();

	/** Release the shift key to disable sprinting */
	void ShiftKeyUp();

	/** CameraBoom positions the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;

	/** FollowCamera is the third person camera that follows the player from behind, and might molest the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;
	
	/** BaseTurnRate to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/**
	 *
	 * Player Stats
	 *
	 */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 Coins;

	void DecrementHealth(float A);

	void Die();

	void IncrementCoins(int32 N);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for forwards/backwards input */
	void MoveForward(float Value);
	/** Called for Side-to-Side input */
	void MoveRight(float Value);

	/** Called via input to turn at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	bool bInteractOn;
	void InteractOn();
	void InteractOff();

	// Need to implement this
	void PrimaryAttackOn();
	void PrimaryAttackOff();
	void SecondaryAttackOn();
	void SecondaryAttackOff();


	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	FORCEINLINE void SetEquippedWeapon(AWeapon* WeaponToSet) { EquippedWeapon = WeaponToSet; }
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	bool bPrimaryAttacking;

	bool bSecondaryAttacking;

	void LightAttack();

	UFUNCTION(BlueprintCallable)
	void LightAttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;
};