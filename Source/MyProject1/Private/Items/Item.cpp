// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "MyProject1/HeaderFile.h" 
#include "Components/SphereComponent.h"
#include "Characters/WarriorCharacter.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());

}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	// UE_LOG(LogTemp,Warning,TEXT("AItem::BeginPlay() called"));
	//
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow, TEXT("AItem::BeginPlay() called"));
	// }

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
 
	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	
	DRAW_SPHERE(StartLocation);
	DRAW_LINE(StartLocation, StartLocation + ForwardVector * 500.f);
	
}


void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWarriorCharacter* WarriorCharacter = Cast<AWarriorCharacter>(OtherActor);
	if (WarriorCharacter)
	{
		WarriorCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWarriorCharacter* WarriorCharacter = Cast<AWarriorCharacter>(OtherActor);
	if (WarriorCharacter)
	{
		WarriorCharacter->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

