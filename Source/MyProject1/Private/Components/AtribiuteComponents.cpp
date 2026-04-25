// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AtribiuteComponents.h"

UAtribiuteComponents::UAtribiuteComponents()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAtribiuteComponents::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAtribiuteComponents::ReciveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

float UAtribiuteComponents::GetHealthPercent() const
{
	return Health / MaxHealth;
}

bool UAtribiuteComponents::IsAlive() const
{
	return Health > 0.f;
}


void UAtribiuteComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

