// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineItems/MyAmmoPickUp.h"

#include "Characters/Olaf.h"

void AMyAmmoPickUp::ApplyPickUp(AOlaf* Character)
{
	Super::ApplyPickUp(Character);
	if (Character)
	{
		Character->AddAmmo(AmmoAmount);
	}
}
