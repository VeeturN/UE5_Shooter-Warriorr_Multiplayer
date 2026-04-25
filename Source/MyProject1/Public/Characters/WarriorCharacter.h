#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Characters/CharacterType.h"
#include "WarriorCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AItem;
class UAnimMontage;
class AWeapon;

UCLASS()
class MYPROJECT1_API AWarriorCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWarriorCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void Jump() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Online
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	//Callback input
	
	UPROPERTY(EditAnywhere,Category="Input")
	UInputMappingContext* WarriorMappingContext;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* WarriorMoveAction;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* WarriorLookingAction;
	
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* WarriorJumpAction;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* DodgeAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	//Play Montage function
	void PlayMontageSection();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	bool CanAttack();
	void PlayEquipmentMontage(FName SectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void EquipSword();

	UFUNCTION(BlueprintCallable)
	void EndEquping();


private:
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;


	

	/**
	 *Animation Montages
	 */
	
	UPROPERTY(EditDefaultsOnly,Category="Montage")
	UAnimMontage* CombatMontage;

	UPROPERTY(EditDefaultsOnly,Category="Montage")
	UAnimMontage* EquipMontage;

	// Online
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon, VisibleAnywhere,Category="Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_ActionState, BlueprintReadWrite, Category="Warrior Character", meta=(AllowPrivateAccess="true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Warrior Character")
	ECharacterState State = ECharacterState::ECS_Unequipped;

	



	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_ActionState();


public:
	FORCEINLINE void SetOverlappingItem(AItem* Item)
	{
		OverlappingItem = Item;
	}
	FORCEINLINE ECharacterState GetCharacterState() const
	{
		return State;
	}
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionDisable(ECollisionEnabled::Type CollisionEnabled);
};
