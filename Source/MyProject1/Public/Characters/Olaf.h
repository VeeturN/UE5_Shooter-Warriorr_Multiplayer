// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" 
#include "Olaf.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UENUM(BlueprintType)
enum class EPlayerEquipState : uint8
{
	EPES_Unequipped UMETA(DisplayName="Unequipped"),
	EPES_Equipped UMETA (DisplayName="Equipped")
};

UENUM(BlueprintType)
enum class EPlayerStateNormal : uint8
{
	EPS_Normal UMETA(DisplayName="Normal"),
	EPS_Reloading UMETA (DisplayName="Reloading"),
	EPS_Aiming UMETA (DisplayName="Aiming")
};

UCLASS()
class MYPROJECT1_API AOlaf : public ACharacter
{
	GENERATED_BODY()

public:
	AOlaf();
	virtual void Jump() override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	float DefaultSpringArmLength;
	
	UPROPERTY(EditDefaultsOnly,Category="Combat")
	float AimSpringArmLength=250.f;
	UPROPERTY(EditDefaultsOnly,Category="Combat")
	float ZoomInterpSeeed=6.f;
	
protected:
	virtual void BeginPlay() override;
	
	//Enum
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	EPlayerEquipState PlayerEquipState= EPlayerEquipState::EPES_Unequipped;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerStateNormal, BlueprintReadOnly, Category = "Combat")
	EPlayerStateNormal PlayerStateNormal= EPlayerStateNormal::EPS_Normal;
	
	UFUNCTION()
	void OnRep_PlayerStateNormal();
	

	/** INPUT SECTION **/
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LeftMouseAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RightMouseAction;
	

	// Funkcje Inputu
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed(const FInputActionValue& Value); 
	void Attack(const FInputActionValue& Value);   
	void ReloadInput(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);
	void AimStart(const FInputActionValue& Value);
	void AimStop(const FInputActionValue& Value);
	
	// Timer Fire
	FTimerHandle FireTimerHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireDelay=0.5f;
	
	bool bCanFire=true;
	
	void ResetFire();
	
	//Ammo 
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxAmmo=5;
	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Combat")
	int32 CurrentAmmo;
	
	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Combat")
	int32 CurrentAmmoInBag=10;
	
	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Combat")
	bool bIsReloading= false;
	void FinishReload();
	
	//Weapon
	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Combat")
	class AOnlineWeapon* EquippedWeapon;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	class AOnlineWeapon* OverlappingWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class UUserWidget> CrosshairClass;
	UPROPERTY()
	class UUserWidget* CrosshairWidget;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
	float CrosshairXOffset=-100.f;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadReactMontage;
	
	//sprint
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float SprintSpeed=800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float WalkSpeed=400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MaxStamina=100.f;
	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Combat")
	float CurrentStamina;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float StaminaDrainRate=15.f;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float StaminaRegenRate=10.f;
	
	bool bWantsToSprint=false;
	bool bIsresting=false;
	
	
	
	//WIget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PlayerHudClass;
	
	
	//server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_EKeyPressed();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float CurrentHealth;
	
	UFUNCTION()
	void OnRep_CurrentHealth();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_PlayAttackEffect();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Death();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_SprintStart();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_SprintStop();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_AimStart();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_AimStop();
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastm_PlayHitReact();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Reload();

public: 
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetHealthPercent() const;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	int32 GetCurrentAmmo() const;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	int32 GetMaxAmmo() const;
	
	void AddAmmo(int32 Amount);
	
	void AddHealth(int32 Health);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	int32 GetCurrentAmmoInBag() const;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	float GetStaminaPercent() const;
	
	//Weapon
	void SetOverlappingWeapon(AOnlineWeapon* Weapon);
	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipWeapon();
	
	//Enum
	UFUNCTION(BlueprintCallable)
	EPlayerEquipState GetPlayerEquipState() const;
	
	UFUNCTION(BlueprintCallable)
	EPlayerStateNormal GetPlayerNormalState() const;

private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;
	
	
	//Serwer

};