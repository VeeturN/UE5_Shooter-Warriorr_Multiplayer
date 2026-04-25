// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	 GeometryCollectionComp=CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComp"));
	 RootComponent=GeometryCollectionComp;
	 GeometryCollectionComp->SetGenerateOverlapEvents(true);
	GeometryCollectionComp->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GeometryCollectionComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);

	CaspsuleComponent=CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CaspsuleComponent->SetupAttachment(GetRootComponent());
	CaspsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CaspsuleComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	
	

	
}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (bBroken) return;
	bBroken=true;
	UWorld* TheWorld=GetWorld();
	if (TheWorld && TreasureClass.Num()>0)
	{
		int32 RandomIndex=FMath::RandRange(0,TreasureClass.Num()-1);
		TheWorld->SpawnActor<ATreasure>(TreasureClass[RandomIndex],GetActorLocation()+FVector(0.f,0.f,50.f),FRotator(0.f,0.f,0.f));
	}
}


