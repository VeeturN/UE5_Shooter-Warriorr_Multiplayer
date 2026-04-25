
#include "Enemy/Enemy.h"
#include "Enemy/Enemy.h"
#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AtribiuteComponents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponents2.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent() -> SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	AtribiuteComponents = CreateDefaultSubobject<UAtribiuteComponents>(TEXT("AtribiuteComponents"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponents2>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()-> bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	PawnSensingComponent=CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensingComponent->SightRadius=4000.f;
	PawnSensingComponent->SetPeripheralVisionAngle(70.f);

	
	
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	EnemyController=Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if (AnimInstance && DeadReact)
	{
		AnimInstance->Montage_Play(DeadReact);
		const int32 SectionCount=DeadReact->GetNumSections();
		const int32 Selection=FMath::RandRange(0,1);
		FName SectionName;
		switch (Selection)
		{
		case 0:
			SectionName=FName("Death1");
			DeathPose=EDeathPose::EAS_Death1;
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Witaj świecie"));
			break;
		case 1:
			SectionName=FName("Death2");
			DeathPose=EDeathPose::EAS_Death2;
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Witaj CFELU"));
			break;
		default:
			break;
		}
		
		AnimInstance->Montage_JumpToSection(SectionName,DeadReact);

		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(3.f);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target==nullptr) return false;
	const double DistaceToTarget = (Target->GetActorLocation()-GetActorLocation()).Size();
	return DistaceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController==nullptr || Target==nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(5.f);
	EnemyController->MoveTo(MoveRequest);

}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets)
	{
		if (Target!=PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	const int32 NumPatrolTargets=ValidTargets.Num();
	if (NumPatrolTargets>0)
	{
		const int32 TargetIndex=FMath::RandRange(0, NumPatrolTargets-1);
		return ValidTargets[TargetIndex];
	}
	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState==EEnemtState::EES_Chasing) return;
	if (SeenPawn->ActorHasTag(FName("Warrior")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed=400.f;
		CombatTarget=SeenPawn;

		if (EnemyState!=EEnemtState::EES_Attacking)
		{
			EnemyState=EEnemtState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
		
	}
}


void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReact)
	{
		AnimInstance->Montage_Play(HitReact);
		AnimInstance->Montage_JumpToSection(SectionName,HitReact);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	const float DotProduct = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(DotProduct);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta = -Theta;
	}

	FName Section = FName("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	PlayHitReactMontage(Section);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget,CombatRadius))
	{
		CombatTarget=nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState=EEnemtState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed=100.f;
		MoveToTarget(PatrolTarget);
	}
	else if (!InTargetRange(CombatTarget,AttackRadius) && EnemyState!=EEnemtState::EES_Chasing)
	{
		EnemyState=EEnemtState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed=300.f;
		MoveToTarget(CombatTarget);
	}
	else if (InTargetRange(CombatTarget,AttackRadius) && EnemyState!=EEnemtState::EES_Attacking)
	{
		EnemyState=EEnemtState::EES_Attacking;
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget,PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 3.f);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState>EEnemtState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
	if (AtribiuteComponents && AtribiuteComponents->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}
	// const float Radius = 30.f;
	// const int32 Segments = 12;
	// const FColor Color = FColor::Red;
	// const bool bPersistentLines = false;
	// const float LifeTime = 2.0f;
	//
	// if (UWorld* World = GetWorld())
	// {
	// 	DrawDebugSphere(World, ImpactPoint, Radius, Segments, Color, bPersistentLines, LifeTime);
	// }
	
	// UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+CrossProduct*100.f,5.f,FColor::Purple,5.f);
	// 	
	//
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Yellow, FString::Printf(TEXT("Theta: %f"), Theta));
	// }
	// UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+Forward*60.f,5.f,FColor::Red,5.f);
	// UKismetSystemLibrary::DrawDebugArrow(this,GetActorLocation(),GetActorLocation()+ToHit*60.f,5.f,FColor::Green,5.f);
		

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint);
	}
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			HitParticles,
			ImpactPoint
			);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	if (AtribiuteComponents && HealthBarWidget)
	{
		AtribiuteComponents->ReciveDamage(DamageAmount);
		HealthBarWidget->SetHealthBarPercent(AtribiuteComponents->GetHealthPercent());
	}
	CombatTarget=EventInstigator->GetPawn();
	EnemyState=EEnemtState::EES_Chasing;
	MoveToTarget(CombatTarget);
	return DamageAmount;
}

