// Fill out your copyright notice in the Description page of Project Settings.


#include "LAN/LAN_Menu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void ULAN_Menu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	FInputModeUIOnly InputMode;
	GetOwningPlayer() -> SetInputMode(InputMode);
	GetOwningPlayer() -> SetShowMouseCursor(true);
	
	Button_Host->OnClicked.AddDynamic(this, &ULAN_Menu::HostButtonClicked);
	Button_Join->OnClicked.AddDynamic(this, &ULAN_Menu::JoinButtonClicked);
}

void ULAN_Menu::HostButtonClicked()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer() -> SetInputMode(InputMode);
	GetOwningPlayer() -> SetShowMouseCursor(false);
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, World, true, "listen");
}

void ULAN_Menu::JoinButtonClicked()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer() -> SetInputMode(InputMode);
	GetOwningPlayer() -> SetShowMouseCursor(false);
	
	const FString Adders = TextBox_IPAddress->GetText().ToString();
	UGameplayStatics::OpenLevel(this,  *Adders);
}
