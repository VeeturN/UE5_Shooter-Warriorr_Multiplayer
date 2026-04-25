// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterType.h"
#include "WarriorAnimInstace.generated.h"


/**
 * 
 */
UCLASS()
class MYPROJECT1_API UWarriorAnimInstace : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite)
	class AWarriorCharacter* WarriorCharacter;
	UPROPERTY(BlueprintReadWrite,Category=Movement)
	class UCharacterMovementComponent* WarriorMovementComponent;
	UPROPERTY(BlueprintReadWrite,Category=Movement)
	float GroundSpeed;
	UPROPERTY(BlueprintReadWrite,Category=Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly,Category="Movement | Character state")
	ECharacterState State;
	
};
