// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterfaces.h"
#include "Characters/CharacterType.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAtribiuteComponents;
class UHealthBarComponents2;
class UPawnSensingComponent;

UCLASS()
class MYPROJECT1_API AEnemy : public ACharacter, public IHitInterfaces
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

	void Die();
	bool InTargetRange(AActor* Target,double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	//Play montage function

	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose=EDeathPose::EAS_Alive;
	

private:
	UPROPERTY(VisibleAnywhere,Category="Components")
	UAtribiuteComponents* AtribiuteComponents;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UHealthBarComponents2* HealthBarWidget;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComponent;
	
	
	
	/**
 *Animation Montages
 */
	UPROPERTY(EditDefaultsOnly,Category="Montage")
	UAnimMontage* HitReact;

	UPROPERTY(EditDefaultsOnly,Category="Montage")
	UAnimMontage* DeadReact;
	
	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Particles")
	UParticleSystem* HitParticles;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius=400.f;
	UPROPERTY(EditAnywhere)
	double PatrolRadius=200.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius=150.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
	void CheckCombatTarget();
	void CheckPatrolTarget();

	//Navigations
	UPROPERTY()
	class AAIController* EnemyController;
	
	
	UPROPERTY(EditInstanceOnly, Category="AI Components")
	AActor* PatrolTarget;
	
	UPROPERTY(EditInstanceOnly, Category="AI Components")
	TArray<AActor*> PatrolTargets;

	EEnemtState EnemyState=EEnemtState::EES_Patrolling;
	


public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
