// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponents2.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT1_API UHealthBarComponents2 : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthBarPercent(float Percent);
private:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
