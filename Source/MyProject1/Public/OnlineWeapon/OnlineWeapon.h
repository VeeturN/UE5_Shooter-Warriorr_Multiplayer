// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnlineWeapon.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial"),
	EWS_Equipped UMETA (DisplayName="Equipped")
};

UCLASS()
class MYPROJECT1_API AOnlineWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AOnlineWeapon();
	virtual void Tick(float DeltaTime) override;
	void Equip(USceneComponent* InParent, FName InSocketName,AActor* NewOwner, APawn* NewInstigator);
protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UStaticMeshComponent* BaseMesh; 

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);	
public:
	FVector GetMuzzleLocation() const;
	

};
