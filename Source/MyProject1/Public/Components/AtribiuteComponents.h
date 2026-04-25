// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AtribiuteComponents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT1_API UAtribiuteComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAtribiuteComponents();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere,Category="Atributtes")
	float Health;
	UPROPERTY(EditAnywhere,Category="Atributtes")
	float MaxHealth;
public:
	void ReciveDamage(float Damage);
	float GetHealthPercent() const;
	bool IsAlive() const;


		
};
