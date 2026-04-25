// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineGame/MyGameMode.h"

void AMyGameMode::PlayerDied(AController* Controller)
{
	if (Controller)
	{
		Controller->UnPossess();
		
		FTimerHandle RespawnTimer;
		FTimerDelegate RespawnDelegate;
		
		RespawnDelegate.BindUObject(
			this,
			&AMyGameMode::RespawnPlayer,
			Controller
			);
		
		GetWorldTimerManager().SetTimer(RespawnTimer, RespawnDelegate, 5.0f, false);
		
		UE_LOG(LogTemp, Warning, TEXT("Gracz zginal! Respawn za 5 sekund..."));
	}
}

void AMyGameMode::RespawnPlayer(AController* Controller)
{
	if (Controller)
	{
		RestartPlayer(Controller);
	}
}
