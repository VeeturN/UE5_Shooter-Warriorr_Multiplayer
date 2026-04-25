// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickUp.generated.h"

class USphereComponent;
class AOlaf;

UCLASS()
class MYPROJECT1_API ABasePickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePickUp();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,Category="Components")
	USphereComponent* OverlapSphere;

	UPROPERTY(VisibleAnywhere,Category="Components")
	UStaticMeshComponent* PickUpMesh;
	
	virtual void ApplyPickUp(AOlaf* Character);
	
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
private:
	


};
