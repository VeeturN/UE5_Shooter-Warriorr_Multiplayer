
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterfaces.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
class UCapsuleComponent;

UCLASS()
class MYPROJECT1_API ABreakableActor : public AActor, public IHitInterfaces
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCapsuleComponent* CaspsuleComponent;
private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollectionComp;

	UPROPERTY(EditAnywhere, Category="Treasure")
	TArray<TSubclassOf<ATreasure>> TreasureClass;

	bool bBroken = false;
};
