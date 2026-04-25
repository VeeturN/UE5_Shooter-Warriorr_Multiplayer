// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineItems/MyHealthPickUp.h"

#include "Characters/Olaf.h"

void AMyHealthPickUp::ApplyPickUp(AOlaf* Character)
{
	Super::ApplyPickUp(Character);

	if (Character)
	{
		Character->AddHealth(20);
	}
	
}
