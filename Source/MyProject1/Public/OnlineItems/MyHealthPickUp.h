// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineItems/BasePickUp.h"
#include "MyHealthPickUp.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT1_API AMyHealthPickUp : public ABasePickUp
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category="PickUp")
	int32 HealthAmount=30;
	
	virtual void ApplyPickUp(AOlaf* Character) override;
	
};
