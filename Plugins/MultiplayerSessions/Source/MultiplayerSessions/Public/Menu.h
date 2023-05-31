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
	void MenuSetup(int32 NumberOfPublicConnections = 8, FString TypeOfMach = FString(TEXT("FreeForAll")));

protected:

	//Existing virtual function called on system initialize
	virtual bool Initialize()override;
	//The NativeDestruct() function is called when the object is being destroyed or deallocated.
	virtual void NativeDestruct() override;

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

	//Undo all all input mode changes that we aplied in menusetup
	void MenuTearDown();

	//
	// Accessing the multiplayerSession class: Inherits from UGameInstanceSubsystem class
	// note: The subsystem designed to handle all online session functionality
	//
	class UMultiplayerSessionsSubsystem* multiplayerSessionsSubsystem;
	
	//variables of controll
	int32 numPublicConnections{8};
	FString matchType{TEXT("FreeForAll")};
	
};
