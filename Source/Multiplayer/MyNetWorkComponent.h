// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//OnlineSessions
#include "Interfaces/OnlineSessionInterface.h"
//OnlineSessions End
#include "MyNetWorkComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_API UMyNetWorkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyNetWorkComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region LAN TESTING

public:

	UFUNCTION(BlueprintCallable)
	void OpenLobby();

	UFUNCTION(BlueprintCallable)
	void CallOpenLevel(const FString& address);

	UFUNCTION(BlueprintCallable)
	void CallOpenLevelClienteTravel(const FString& address);

#pragma endregion

#pragma region Testing Online Sessions 

public:

	class IOnlineSubsystem* onlineSubsystem;//Forward declararion for the online subsystem
	IOnlineSessionPtr onlineSessionInterface;
	TSharedPtr<class FOnlineSessionSettings> sessionSettings; //https://docs.unrealengine.com/4.27/en-US/API/Plugins/OnlineSubsystem/FOnlineSessionSettings/
	TSharedPtr<class FOnlineSessionSearch> sessionSearch; //https://docs.unrealengine.com/4.27/en-US/API/Plugins/OnlineSubsystem/FOnlineSession/


protected:

	//Calling from the editor UE5 via interface or input
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();
	
	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

	//Callback function
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnjoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:

	/// <summary>
	/// This delegate will be included to the sessionInterface delegate list
	/// then this needs one or more callback functions to reponde when the delegate is fired
	/// note: Check the signature for call back functions
	/// note 2:  Initialize Delegate and bind callback function in the initialization list of the constructor
	/// </summary>
	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;

#pragma endregion

};
