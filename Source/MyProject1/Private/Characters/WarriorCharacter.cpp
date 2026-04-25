// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Engine/Engine.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"


AWarriorCharacter::AWarriorCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 300.0f;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(WarriorMappingContext, 0);
		}
	}
	Tags.Add(FName("Warrior"));
}

void AWarriorCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState!=EActionState::EAS_Unoccupied) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AWarriorCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	AddControllerPitchInput(MovementVector.Y);
	AddControllerYawInput(MovementVector.X);
}

void AWarriorCharacter::EKeyPressed(const FInputActionValue& Value)
{
	AWeapon* OverlapingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlapingWeapon)
	{
		OverlapingWeapon->Equip(GetMesh(), FName("RRightHandSocket"), this, this->GetInstigator());
		State = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem=nullptr;
		EquippedWeapon = OverlapingWeapon;
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipmentMontage(FName("UnEquip"));
			State = ECharacterState::ECS_Unequipped;
			ActionState=EActionState::EAS_Equiping;
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, "Disarmed");
			}
		}
		else if (CanArm())
		{
			PlayEquipmentMontage(FName("Equip"));
			State = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState=EActionState::EAS_Equiping;
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Yellow, "Armed");
		}
	}
}

void AWarriorCharacter::Attack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		PlayMontageSection();
		ActionState=EActionState::EAS_Attacking;
	}
	
}

void AWarriorCharacter::PlayMontageSection()
{
	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage);
		const int32 Selection=FMath::RandRange(0,1);
		switch (Selection)
		{
		case 0:
			AnimInstance->Montage_JumpToSection(FName("Attack1"),CombatMontage);
			break;
		case 1:
			AnimInstance->Montage_JumpToSection(FName("Attack2"),CombatMontage);
			break;
		default:
			break;
		}
	}
}

void AWarriorCharacter::PlayEquipmentMontage(FName SectionName)
{
	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName,EquipMontage);
	}
}

void AWarriorCharacter::AttackEnd()
{
	ActionState=EActionState::EAS_Unoccupied;
}

bool AWarriorCharacter::CanAttack()
{
	return ActionState==EActionState::EAS_Unoccupied &&
							State!=ECharacterState::ECS_Unequipped;
}


bool AWarriorCharacter::CanDisarm()
{
	return ActionState==EActionState::EAS_Unoccupied &&
		State!=ECharacterState::ECS_Unequipped;
}

bool AWarriorCharacter::CanArm()
{
	return ActionState==EActionState::EAS_Unoccupied &&
		State==ECharacterState::ECS_Unequipped &&
			EquippedWeapon;
}

void AWarriorCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("BackSwordLocationSocket"));
	}
}

void AWarriorCharacter::EquipSword()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RRightHandSocket"));
	}
}

void AWarriorCharacter::EndEquping()
{
	ActionState=EActionState::EAS_Unoccupied;
}

void AWarriorCharacter::SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
}

void AWarriorCharacter::SetWeaponCollisionDisable(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void AWarriorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarriorCharacter::Jump()
{
	Super::Jump();
}

void AWarriorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(WarriorMoveAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Move);
		EnhancedInputComponent->BindAction(WarriorLookingAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Look);
		EnhancedInputComponent->BindAction(WarriorJumpAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Attack);
		// EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AWarriorCharacter::Jump);
	}
}

void AWarriorCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,EquippedWeapon);
	DOREPLIFETIME(ThisClass,ActionState);
	DOREPLIFETIME(ThisClass,State);
}

void AWarriorCharacter::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetActorEnableCollision(false);
	}
}

void AWarriorCharacter::OnRep_ActionState()
{
	
}


