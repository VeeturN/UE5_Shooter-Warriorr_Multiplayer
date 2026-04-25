// Fill out your copyright notice in the Description page of Project Settings.


#include "OnlineWeapon/OnlineWeapon.h"

#include "Characters/Olaf.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AOnlineWeapon::AOnlineWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates=true;
	SetReplicateMovement(true);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh")); // Nowa nazwa
	RootComponent = BaseMesh;
	BaseMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BaseMesh->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	AreaSphere=CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	WeaponState=EWeaponState::EWS_Initial;
	
}


void AOnlineWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AOnlineWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AOnlineWeapon::OnSphereEndOverlap);
	}
}

void AOnlineWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOnlineWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOlaf* OlafCharacter= Cast<AOlaf>(OtherActor);
	if (OlafCharacter)
	{
		OlafCharacter->SetOverlappingWeapon(this);
	}
}

void AOnlineWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AOlaf* OlafCharacter= Cast<AOlaf>(OtherActor);
	if (OlafCharacter)
	{
		OlafCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AOnlineWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,true);
	BaseMesh->AttachToComponent(InParent,TransformRules,InSocketName);
	
	WeaponState=EWeaponState::EWS_Equipped;
	
	if (AreaSphere)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


FVector AOnlineWeapon::GetMuzzleLocation() const
{
	if (BaseMesh)
	{
		return BaseMesh->GetSocketLocation("MazzleSocket");
	} 
	return GetActorLocation();
}

