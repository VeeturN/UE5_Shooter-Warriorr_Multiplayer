// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Olaf.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h" // Do zadawania obrażeń (ApplyDamage)
#include "Engine/World.h"           // Do LineTrace
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "OnlineGame/MyGameMode.h"
#include "OnlineWeapon/OnlineWeapon.h"

AOlaf::AOlaf()
{
    bReplicates = true;
    
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    // Konfiguracja Kamery i SpringArm
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->TargetArmLength = 300.0f;
    
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
    
    CurrentHealth = 100.0f;
    
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOlaf::BeginPlay()
{
    Super::BeginPlay();
    

    if (SpringArmComponent)
    {
        DefaultSpringArmLength = SpringArmComponent->TargetArmLength;
    }
    
    if (HasAuthority())
    {
        CurrentStamina=20.f;
        CurrentAmmo=MaxAmmo;
    }
    
    if (IsLocallyControlled() && PlayerHudClass && CrosshairClass)
    {
        UUserWidget* HUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHudClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
        
        CrosshairWidget= CreateWidget<UUserWidget>(GetWorld(), CrosshairClass);
        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport();
            CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        
    }
}
void AOlaf::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float TargetFOV = (PlayerStateNormal == EPlayerStateNormal::EPS_Aiming) ? 80.0f : 90.0f;
    float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSeeed);
    CameraComponent->SetFieldOfView(NewFOV);
    if (IsLocallyControlled())
    {
        float TargetAimLength = DefaultSpringArmLength;
        if (PlayerStateNormal==EPlayerStateNormal::EPS_Aiming)
        {
            TargetAimLength = AimSpringArmLength;
        }
        
        float NewLength = FMath::FInterpTo(
            SpringArmComponent->TargetArmLength,
            TargetAimLength,
            DeltaTime,
            ZoomInterpSeeed
        );
        SpringArmComponent->TargetArmLength = NewLength;
    }
    
    
    if (HasAuthority() || IsLocallyControlled())
    {
        bool bIsMoving = GetVelocity().SizeSquared() > 5.f;

        if (CurrentStamina>20.f)
        {
            bIsresting=false;
        }
        
        if (bWantsToSprint && bIsMoving && CurrentStamina > 0.f && !bIsresting)
        {
            GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
            
            if (HasAuthority())
            {
                CurrentStamina-= StaminaDrainRate * DeltaTime;
                if (CurrentStamina<0.f)
                {
                    CurrentStamina=0.f;
                    bIsresting=true;
                }
            }
        }
        else
        {
            GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
            
            if (HasAuthority() && CurrentStamina < MaxStamina) 
            {
                CurrentStamina+= StaminaRegenRate * DeltaTime;
                if (CurrentStamina>MaxStamina)
                {
                    CurrentStamina=MaxStamina;
                }
            }
        }
    }
    
}

void AOlaf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 1);
            }
        }
    }
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOlaf::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOlaf::Look);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AOlaf::Jump);
        EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AOlaf::EKeyPressed);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AOlaf::Attack);
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AOlaf::ReloadInput);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOlaf::SprintStart);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOlaf::SprintStop);
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Started, this, &AOlaf::AimStart);
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Completed, this, &AOlaf::AimStop);
    }
}

void AOlaf::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(ForwardDirection, MovementVector.X);
    
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(RightDirection, MovementVector.Y);
}
void AOlaf::Jump()
{
    Super::Jump();
}

float AOlaf::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // Mamy autorytet? (Serwer)
    if (HasAuthority())
    {
        // Odejmujemy życie
        CurrentHealth -= ActualDamage;
        
        // Logika śmierci (taka sama jak wcześniej)
        if (CurrentHealth <= 0.0f)
        {
            CurrentHealth = 0.0f;
            MulticastRPC_Death();
            
            AMyGameMode* GM = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
            if (GM)
            {
                GM->PlayerDied(GetController());
            }
        }
        else
        {
            NetMulticastm_PlayHitReact();
        }

        // Pamiętaj o wymuszeniu replikacji dla serwera!
        OnRep_CurrentHealth();

        UE_LOG(LogTemp, Warning, TEXT("Oberwalem za: %f od %s"), ActualDamage, *DamageCauser->GetName());
    }

    return ActualDamage;
}

void AOlaf::Look(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    AddControllerPitchInput(MovementVector.Y);
    AddControllerYawInput(MovementVector.X);
}

void AOlaf::EKeyPressed(const FInputActionValue& Value)
{
        ServerRPC_EquipWeapon();
}

void AOlaf::Attack(const FInputActionValue& Value)
{
    if (bCanFire == false || CurrentAmmo<=0 || PlayerStateNormal==EPlayerStateNormal::EPS_Reloading)
    {
        return;
    }
    ServerRPC_Fire();
    
    bCanFire = false;
    
    GetWorldTimerManager().SetTimer(
        FireTimerHandle,    // Uchwyt timera
        this,               // Obiekt
        &AOlaf::ResetFire,  // Funkcja do wywołania
        FireDelay,          // Czas
        false               // Czy pętla? False - tylko raz
    );
}

void AOlaf::ReloadInput(const FInputActionValue& Value)
{
    if (CurrentAmmo<MaxAmmo && PlayerStateNormal != EPlayerStateNormal::EPS_Reloading)
    {
        ServerRPC_Reload();
    }
}

void AOlaf::SprintStart(const FInputActionValue& Value)
{
    bWantsToSprint = true;
    
    ServerRPC_SprintStart();
}

void AOlaf::SprintStop(const FInputActionValue& Value)
{
    bWantsToSprint=false;
    ServerRPC_SprintStop();
}

void AOlaf::AimStart(const FInputActionValue& Value)
{
    ServerRPC_AimStart();
}

void AOlaf::AimStop(const FInputActionValue& Value)
{
    ServerRPC_AimStop();
}

void AOlaf::ResetFire()
{
    bCanFire = true;
}

void AOlaf::FinishReload()
{
    if (HasAuthority())
    {
        int32 tmpAmoNeeded=MaxAmmo - CurrentAmmo;
        if (CurrentAmmoInBag>=tmpAmoNeeded)
        {
            CurrentAmmo+=tmpAmoNeeded;
            CurrentAmmoInBag-=tmpAmoNeeded;
        }
        else
        {
            CurrentAmmo+=CurrentAmmoInBag;
            CurrentAmmoInBag=0;
        }
        
        PlayerStateNormal= EPlayerStateNormal::EPS_Normal;
        
        if (IsLocallyControlled())
        {
            OnRep_PlayerStateNormal(); // To schowa celownik u Hosta
        }
        
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange, TEXT("Przeladowano!"));
    }
}

void AOlaf::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AOlaf, CurrentHealth);
    DOREPLIFETIME(AOlaf, CurrentAmmo);
    DOREPLIFETIME(AOlaf, CurrentAmmoInBag);
    DOREPLIFETIME(AOlaf, CurrentStamina);
    DOREPLIFETIME(AOlaf, PlayerEquipState);
    DOREPLIFETIME(AOlaf, PlayerStateNormal)
    DOREPLIFETIME(AOlaf, EquippedWeapon)
}


void AOlaf::ServerRPC_EKeyPressed_Implementation()
{
    CurrentHealth-=10.f;
    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        MulticastRPC_Death();
    }
        
    OnRep_CurrentHealth();
        
    FString Msg = FString::Printf(TEXT("Olaf oberwal! HP: %f"), CurrentHealth);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
}

void AOlaf::ServerRPC_Fire_Implementation()
{
    if (CurrentAmmo<=0 || PlayerStateNormal==EPlayerStateNormal::EPS_Reloading || !EquippedWeapon) 
        return;
    
    CurrentAmmo--;
    
    MulticastRPC_PlayAttackEffect();
    
    // 1. Definiujemy parametry strzału
    FVector CameraLocation;
    FRotator CameraRotation;
    
    // Pobieramy pozycję oczu i rotację kontrolera (gdzie gracz patrzy)
    // Używamy ControllerRotation, bo kamera jest na kliencie, a my jesteśmy na serwerze!
    if (GetController())
    {
        GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
    }
    else
    {
        // Fallback jeśli nie ma kontrolera (np. bot)
        CameraLocation = GetActorLocation();
        CameraRotation = GetActorRotation();
    }

    // Strzał na 100 metrów (10 000 jednostek)
    FVector CameraTrackEnd = CameraLocation + (CameraRotation.Vector() * 20000.0f);

    FHitResult CameraHit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // Nie chcemy trafić sami siebie w nos
    QueryParams.AddIgnoredActor(EquippedWeapon);
    bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
        CameraHit,
        CameraLocation,
        CameraTrackEnd,
        ECC_Visibility, // Kanał kolizji (widoczność)
        QueryParams
    );
    
    FVector TargetPoint = bCameraHit ? CameraHit.Location : CameraTrackEnd;
    
    FVector MuzzleLocation = EquippedWeapon->GetMuzzleLocation();
    FVector ShootDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
    FVector WeaponTraceEnd = MuzzleLocation + (ShootDirection * 20000.0f);
    
    // 2. Konfiguracja LineTrace
    FHitResult WeaponHit;

    // 3. Wykonanie strzału (Raycast)
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        WeaponHit,
        MuzzleLocation,
        WeaponTraceEnd,
        ECC_Visibility, // Kanał kolizji (widoczność)
        QueryParams
    );

    // 4. Debugowa linia (żebyś widział gdzie strzelasz) - widoczna przez 2 sekundy
    DrawDebugLine(GetWorld(), MuzzleLocation, WeaponTraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);

    if (bHit && WeaponHit.GetActor())
    {
        // Trafiliśmy coś!
        AActor* HitActor = WeaponHit.GetActor();
        
        // Sprawdźmy czy to inny Olaf (albo cokolwiek co da się zranić)
        // ApplyDamage wywoła funkcję TakeDamage na ofierze
        UGameplayStatics::ApplyDamage(
            HitActor,
            20.0f,                  // Ilość obrażeń
            GetController(),        // Kto strzelał (Instigator)
            this,                   // Co strzelało (Causer)
            UDamageType::StaticClass()
        );
    }
}

void AOlaf::NetMulticastm_PlayHitReact_Implementation()
{
    if (HitReactMontage)
    {
        PlayAnimMontage(HitReactMontage);
        //UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
    }
}

void AOlaf::Multicast_Reload_Implementation()
{
    if (ReloadReactMontage)
    {   
        PlayAnimMontage(ReloadReactMontage); 
    }
}


void AOlaf::OnRep_CurrentHealth()
{
    // Sprawdzamy, czy jesteśmy serwerem, czy klientem
    FString WhoAmI = HasAuthority() ? TEXT("[SERWER]") : TEXT("[KLIENT]");
    
    // Sprawdzamy czy to 'mój' Olaf (czy ja nim steruję), czy Olaf kolegi
    const bool bIsMine = IsLocallyControlled();
    FString Ownership = bIsMine ? TEXT("(To Ja)") : TEXT("(To Inny Gracz)");

    FString Message = FString::Printf(TEXT("%s%s: O kurde, moje HP to teraz: %f"), *WhoAmI, *Ownership, CurrentHealth);
    
    // Różne kolory dla łatwiejszego odróżnienia
    FColor LogColor = HasAuthority() ? FColor::Red : (bIsMine ? FColor::Green : FColor::Yellow);

    GEngine->AddOnScreenDebugMessage(-1, 5.f, LogColor, Message);
}

void AOlaf::OnRep_PlayerStateNormal()
{
    if (IsLocallyControlled())
    {
        if (PlayerStateNormal == EPlayerStateNormal::EPS_Aiming)
        {
            CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }   
}

void AOlaf::MulticastRPC_PlayAttackEffect_Implementation()
{
    if (FireWeaponMontage)
    {
        PlayAnimMontage(FireWeaponMontage);
    }
    if (EquippedWeapon && MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAttached(
            MuzzleFlash,
            EquippedWeapon->GetRootComponent(),
            TEXT("MazzleSocket"),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            FVector(0.1f),
            EAttachLocation::SnapToTarget,
            true
            );
    }
}

void AOlaf::MulticastRPC_Death_Implementation()
{
    DisableInput(nullptr);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);
}

void AOlaf::ServerRPC_Reload_Implementation()
{
    // PlayerStateNormal != EPS_Aiming w warunku ???
    if (PlayerStateNormal == EPlayerStateNormal::EPS_Reloading || CurrentAmmo == MaxAmmo) return;
    
    PlayerStateNormal = EPlayerStateNormal::EPS_Reloading;
    
    if (IsLocallyControlled())
    {
        OnRep_PlayerStateNormal(); // To schowa celownik u Hosta
    }
    
    FTimerHandle ReloadTimerHandle;
    GetWorldTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &AOlaf::FinishReload,
        2.4f,
        false
    );
    Multicast_Reload();
    
    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange, TEXT("Przeladowuje..."));
}

void AOlaf::ServerRPC_SprintStart_Implementation()
{
    bWantsToSprint = true;
}

void AOlaf::ServerRPC_SprintStop_Implementation()
{
    bWantsToSprint = false;
}
void AOlaf::ServerRPC_EquipWeapon_Implementation()
{
    if (OverlappingWeapon)
    {
        EquippedWeapon=OverlappingWeapon;
        EquippedWeapon->Equip(GetMesh(),TEXT("RightHandSocket"),this,this);
        OverlappingWeapon = nullptr;
        PlayerEquipState = EPlayerEquipState::EPES_Equipped;
    }
}

void AOlaf::ServerRPC_AimStart_Implementation()
{
    if (PlayerStateNormal==EPlayerStateNormal::EPS_Reloading || !EquippedWeapon) return;
    
    PlayerStateNormal = EPlayerStateNormal::EPS_Aiming;
    //GetCharacterMovement()->MaxWalkSpeed = 400.0f;

    if (IsLocallyControlled())
    {
        OnRep_PlayerStateNormal();
    }
    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue, TEXT("Celowanie!"));
}

void AOlaf::ServerRPC_AimStop_Implementation()
{
    if (PlayerStateNormal==EPlayerStateNormal::EPS_Aiming)
    {
        PlayerStateNormal = EPlayerStateNormal::EPS_Normal;
        //GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        if (IsLocallyControlled())
        {
            OnRep_PlayerStateNormal();
        }
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue, TEXT("Przestalem celowac!"));
    }
}


//GETERY SETERY
float AOlaf::GetHealthPercent() const
{
    return CurrentHealth/100.f;
}

int32 AOlaf::GetCurrentAmmo() const
{
    return CurrentAmmo;
}

int32 AOlaf::GetMaxAmmo() const
{
    return MaxAmmo;
}

void AOlaf::AddAmmo(int32 Amount)
{
    if (HasAuthority())
    {
        CurrentAmmoInBag+=Amount;
    }
}

void AOlaf::AddHealth(int32 Health)
{
    if (HasAuthority())
    {
        CurrentHealth+=Health;
        if (CurrentHealth>100.f)
        {
            CurrentHealth=100.f;
        }
        OnRep_CurrentHealth();
    }
}

int32 AOlaf::GetCurrentAmmoInBag() const
{
    return CurrentAmmoInBag;
}

float AOlaf::GetStaminaPercent() const
{
    return CurrentStamina/MaxStamina;
}

void AOlaf::SetOverlappingWeapon(AOnlineWeapon* Weapon)
{
    if (OverlappingWeapon)
    {
        
    }
    
    OverlappingWeapon=Weapon;
}

EPlayerEquipState AOlaf::GetPlayerEquipState() const
{
    return PlayerEquipState;
}
EPlayerStateNormal AOlaf::GetPlayerNormalState() const
{
    return PlayerStateNormal;
}

