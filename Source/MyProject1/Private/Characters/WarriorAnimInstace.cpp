// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorAnimInstace.h"
#include "Characters/WarriorCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UWarriorAnimInstace::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	WarriorCharacter=Cast<AWarriorCharacter>(TryGetPawnOwner());
	if (WarriorCharacter)
	{
		WarriorMovementComponent=WarriorCharacter->GetCharacterMovement();	
	}
	
}

void UWarriorAnimInstace::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (WarriorMovementComponent)
	{
		GroundSpeed=UKismetMathLibrary::VSizeXY(WarriorMovementComponent->Velocity);
		IsFalling=WarriorMovementComponent->IsFalling();
		State = WarriorCharacter->GetCharacterState();
	}
	
}
