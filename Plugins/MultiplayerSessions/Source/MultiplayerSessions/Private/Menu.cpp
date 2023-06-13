// Fill out your copyright notice in the Description page of Project Settings.
#include "Menu.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMach)
{
	//Set member variables
	numPublicConnections = NumberOfPublicConnections;
	matchType = TypeOfMach;

	//Set Viewport
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	//bIsFocusable = true;
	SetIsFocusable(true);

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

	//
	//Bind Call backs to get the response from the Multiplyer subsystem
	//
	if (multiplayerSessionsSubsystem)
	{
		//Acess multiplayersession class/ Delegate inside the class /					Call back function in response
		multiplayerSessionsSubsystem->multiplayerOnCreateSessionComplete.AddDynamic(this,&ThisClass::OncreateSession);
		multiplayerSessionsSubsystem->multiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		multiplayerSessionsSubsystem->multiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		multiplayerSessionsSubsystem->multiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OncreateSession);
		multiplayerSessionsSubsystem->multiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

void UMenu::SetLobbyPath(FString LobbyPath)
{
	pathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
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

	auto ServerEntryClass = serverList->GetEntryWidgetClass();
	UUserWidget* NewServerEntry;
	for (int i = 0; i < 25; i++)
	{
		NewServerEntry = CreateWidget<UUserWidget>(this, ServerEntryClass);
		serverList->AddItem(NewServerEntry);
	}
	//This code is meant to be a test for modifying the string inside of Entries in the list. GetWidgetFromName is not succesfully getting the horizontal box currently. 
	//The current possible errors are the string doesnt match which doesn't seem to be the case and the other is that the HorizontalBox is not a direct child of the NewServerEntry but it appears to me in the blueprint that this is a direct child so IDK.
	//if (NewServerEntry)
	//{
	//	UHorizontalBox* HorizontalBox = Cast<UHorizontalBox>(NewServerEntry->GetWidgetFromName("Horizontal Box"));
	//	if (HorizontalBox)
	//	{
	//		UTextBlock* TextBlock = Cast<UTextBlock>(HorizontalBox->GetChildAt(1));
	//		if (TextBlock)
	//		{
	//			FString TestServerName = "Test Server Name";
	//			TextBlock->SetText(FText::FromString(TestServerName));
	//		}
	//		else
	//		{
	//			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("TextBlock widget not found in ServerEntryClass"));
	//		}
	//	}
	//	else
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to find Horizontal Box"));
	//	}
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to create NewServerEntry"));
	//}
	return true;
}

//It is an override of the base class's destruct function, allowing you to implement your own cleanup logic
void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

#pragma region CALLBACKS for custom delegates residing on MultiplayerSessionsSubsystem

void UMenu::OncreateSession(bool bWasSuccessful)
{
	//On Session Failed
	if (!bWasSuccessful)
	{
		hostButton->SetIsEnabled(true);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Session Failed")));
		}
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Session Created successfully")));
	}
	UWorld* World = GetWorld();
	if (World)
	{
		if (pathToLobby == TEXT("null"))
		{
			pathToLobby = FString("/Game/ThirdPerson/Maps/Lobby?listen");
		}
		World->ServerTravel(pathToLobby);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{

}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{

	if (multiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		if (SettingsValue == matchType)
		{
			multiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() <= 0)
	{
		joinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		joinButton->SetIsEnabled(true);
	}
}

#pragma endregion

void UMenu::HostButtonClicked()
{
	hostButton->SetIsEnabled(false);
	if (multiplayerSessionsSubsystem)
	{
		multiplayerSessionsSubsystem->CreateSession(numPublicConnections, matchType);

	}
}

void UMenu::JoinButtonClicked()
{
	joinButton->SetIsEnabled(false);
	if (multiplayerSessionsSubsystem)
	{
		//Because we are using the Steam DevID '480', there is a high chance of finding many sessions. Therefore, we are searching for a large number of possible sessions.
		multiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	//Remove the widget
	RemoveFromParent();

	//Access player controller to restore input to it
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerControler = World->GetFirstPlayerController();
		if (PlayerControler)
		{
			FInputModeGameOnly InputModeData;
			PlayerControler->SetInputMode(InputModeData);
			PlayerControler->SetShowMouseCursor(false);
		}
	}
}