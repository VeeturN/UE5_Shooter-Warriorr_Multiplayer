// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"

#include "Characters/WarriorCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWarriorCharacter* WarriorCharacter = Cast<AWarriorCharacter>(OtherActor);
	if (WarriorCharacter)
	{
		if (PickSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
					PickSound,
					GetActorLocation()
				);
		}
		Destroy();
	}
}

