// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup();

protected:

	//Existing virtual function called on system initialize
	virtual bool Initialize()override;


private:
	//
	//Variables to acess the buttons in our widget UI
	//https://docs.unrealengine.com/4.27/en-US/API/Runtime/UMG/Components/UButton/
	//
	UPROPERTY(meta = (BindWidget))
	class UButton* hostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* joinButton;

	//
	//Functions to bind the Buttons functionalities
	//
	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();
	//
	// Accessing the multiplayerSession class: Inherits from UGameInstanceSubsystem class
	// note: The subsystem designed to handle all online session functionality
	//
	class UMultiplayerSessionsSubsystem* multiplayerSessionsSubsystem;
	
};
