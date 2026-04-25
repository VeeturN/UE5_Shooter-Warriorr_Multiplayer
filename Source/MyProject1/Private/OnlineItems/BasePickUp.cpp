

#include "OnlineItems/BasePickUp.h"

#include "Characters/Olaf.h"
#include "Components/SphereComponent.h"

ABasePickUp::ABasePickUp()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	OverlapSphere= CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	RootComponent = OverlapSphere;
	
	OverlapSphere->SetSphereRadius(25.0f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	
	PickUpMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	PickUpMesh->SetupAttachment(GetRootComponent());
	PickUpMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUp::OnSphereOverlap);
	}
	
}

void ABasePickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasePickUp::ApplyPickUp(AOlaf* Character)
{
}

void ABasePickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOlaf* OlafCharacter= Cast<AOlaf>(OtherActor);

	if (OlafCharacter)
	{
		ApplyPickUp(OlafCharacter);
		Destroy();
	}
}

