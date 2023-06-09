// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
//OnlineSessions
#include "OnlineSubsystem.h"//included as a dependency for our multiplayer sessions plugin
#include "OnlineSessionSettings.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	createSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	findSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	joinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	destroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	startSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	//Iniatialize subsystem and session interface references
	subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		sessionInterface = subsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Found subsystem %s"), *subsystem->GetSubsystemName().ToString()));
		}
	}
}

#pragma region MENU SYSTEM

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!sessionInterface.IsValid())
	{
		return;
	}

	// To avoid having one session created on top of each other check if the session already exists if so, delete before create a new one.
	auto ExistingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		sessionInterface->DestroySession(NAME_GameSession);
	}
	
	//Store the Delegate in a FDelegateHandle so we can later remove it from the delegate list
	createSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	//Initialize the session settings
	lastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSettings->NumPublicConnections = NumPublicConnections;
	lastSessionSettings->bAllowJoinInProgress = true;
	lastSessionSettings->bAllowJoinViaPresence = true;
	lastSessionSettings->bShouldAdvertise = true;
	lastSessionSettings->bUsesPresence = true;
	lastSessionSettings->bUseLobbiesIfAvailable = true;
	lastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//in Unreal Engine, the function GetPreferredUniqueNetId() is used to retrieve the unique network identifier (NetId) for a local player. It is a member function of the ULocalPlayer class, which represents a player who is locally controlled on the client.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool bSessionSucceed =  sessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *lastSessionSettings);

	//If Create session returns false remove the delegate from the delegate list
	if (!bSessionSucceed)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
		//BroadCast our own custom delegate in response to the menu call
		multiplayerOnCreateSessionComplete.Broadcast(false);
	}


	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Create Session: %d %s "), NumPublicConnections, *MatchType));
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

#pragma endregion

#pragma region CALLBACKS_IN_RESPONSE_TO_EOS


void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//Once we are done with the delegate from the EOS we can remove it from delegate list
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
	}
	//BradCast the signal in response to the menu system
	multiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);

}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

#pragma endregion