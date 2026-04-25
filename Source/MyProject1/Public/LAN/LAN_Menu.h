// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LAN_Menu.generated.h"

class UEditableTextBox;
class UButton;
/**
 * 
 */
UCLASS()
class MYPROJECT1_API ULAN_Menu : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_IPAddress; 
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Host;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Join;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> World;
	
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();
	
};
