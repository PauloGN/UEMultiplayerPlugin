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
		bCreateSessionOnDestroy = true;
		lastNumPublicConnections = NumPublicConnections;
		lastMatchType = MatchType;

		DestroySession();
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
	lastSessionSettings->BuildUniqueId = 1;

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
	if (!sessionInterface)
	{
		return;
	}

	findSessionsCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegate);

	lastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	lastSessionSearch->MaxSearchResults = MaxSearchResults;
	lastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool bFoundSession = sessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), lastSessionSearch.ToSharedRef());

	//Covering failure and reponding to the menu via broad casting
	if (!bFoundSession)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);
		multiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!sessionInterface)
	{
		multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	joinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	bool bJoinSuccessfuly = sessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult);

	if (!bJoinSuccessfuly)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
		multiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!sessionInterface.IsValid())
	{
		multiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	destroySessionCompleteDelegateHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);

	if (!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegateHandle);
		multiplayerOnDestroySessionComplete.Broadcast(false);
	}
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
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);
	}

	if (lastSessionSearch->SearchResults.Num() <= 0)
	{
		multiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	multiplayerOnFindSessionsComplete.Broadcast(lastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
	}

	multiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(lastNumPublicConnections, lastMatchType);
	}
	multiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{

}
#pragma endregion