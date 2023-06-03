// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
//OnlineSessions
#include "Interfaces/OnlineSessionInterface.h"
//OnlineSessions End
#include "MultiplayerSessionsSubsystem.generated.h"

//
//Declaring our own Custum delegates for the MenuClass to bind call backs to
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UMultiplayerSessionsSubsystem();

	//
	// To handle session functionality. The Menu class will call this.
	//
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	//
	//Custom DELEGATES for the Menu Class be able to bind call backs to
	//
	FMultiplayerOnCreateSessionComplete multiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete multiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete multiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete multiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete multiplayerOnStartSessionComplete;

protected:

	//
	//Internal Call backs for the delegates that will be added to the Online Session Interface delegate list
	// these dont need to be called outside this class
	//
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:

	class IOnlineSubsystem* subsystem;
	IOnlineSessionPtr sessionInterface;
	// settings used in the last session
	TSharedPtr<FOnlineSessionSettings> lastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> lastSessionSearch;

	//
	// Delegates to add to the Online Session Interface delegate list.
	// We'll  bind ourr MultiplayerSessionSubsystem internal callback to these
	//
	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;
	FOnStartSessionCompleteDelegate startSessionCompleteDelegate;
	//
	//Delegate Handle will hold the delegate type to clear the delegate list once we are done with the delegate call back function
	//As soon as we add the delegate to the delegate list the function we use to do that will return the FDelegate handle
	//
	FDelegateHandle createSessionCompleteDelegateHandle;
	FDelegateHandle findSessionsCompleteDelegateHandle;
	FDelegateHandle joinSessionCompleteDelegateHandle;
	FDelegateHandle destroySessionCompleteDelegateHandle;
	FDelegateHandle startSessionCompleteDelegateHandle;
	//
	//Check Destroy session
	//
	bool bCreateSessionOnDestroy{ false };
	int32 lastNumPublicConnections;
	FString lastMatchType;
};
