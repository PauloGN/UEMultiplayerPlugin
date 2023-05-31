// Fill out your copyright notice in the Description page of Project Settings.
#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerControler = World->GetFirstPlayerController();

		if (PlayerControler)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerControler->SetInputMode(InputModeData);
			PlayerControler->SetShowMouseCursor(true);
		}
	}

	//Access the Multiplayer subsystem
	//To access the functionality provided by the `multiplayerSession` class,
	//you need to have an instance of the game instance class.
	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		//Through this subsystem we can call functions that will access the online servises
		multiplayerSessionsSubsystem =  GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

}

bool UMenu::Initialize()
{
	// If the base class does not initialize retur false
	if (!Super::Initialize())
	{
		return false;
	}

	if (hostButton)
	{
		hostButton->OnClicked.AddDynamic(this,&ThisClass::HostButtonClicked);
	}

	if (joinButton)
	{
		joinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::HostButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Host Session Button")));
	}

	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->CreateSession(25, FString("FreeForAll"));
	}
}

void UMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Joing Session Button")));
	}

	if (multiplayerSessionsSubsystem)
	{
		//multiplayerSessionsSubsystem->JoinSession(FOnlineSessionSearchResult());
	}
}