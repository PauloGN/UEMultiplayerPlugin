// Fill out your copyright notice in the Description page of Project Settings.


#include "MyNetWorkComponent.h"
#include <Kismet/GameplayStatics.h>
//OnlineSessions
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


// Sets default values for this component's properties
UMyNetWorkComponent::UMyNetWorkComponent() :
	createSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	findSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
	joinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnjoinSessionComplete))
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ONLINE SESSION initialization
	onlineSubsystem = IOnlineSubsystem::Get();
	sessionSettings = MakeShareable(new FOnlineSessionSettings());
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (onlineSubsystem)
	{
		onlineSessionInterface = onlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,  FString::Printf(TEXT("Found subsystem %s"), *onlineSubsystem->GetSubsystemName().ToString()));
		}
	}
}


// Called when the game starts
void UMyNetWorkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMyNetWorkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma region LAN TESTING

void UMyNetWorkComponent::OpenLobby()
{
	UWorld* world = GetWorld();

	if (IsValid(world))
	{
		world->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
	}
}


void UMyNetWorkComponent::CallOpenLevel(const FString& address)
{
	UGameplayStatics::OpenLevel(this, *address);
}

void UMyNetWorkComponent::CallOpenLevelClienteTravel(const FString& address)
{
	APlayerController* playerController = GetOwner()->GetGameInstance()->GetFirstLocalPlayerController();

	if (playerController)
	{
		playerController->ClientTravel(address, TRAVEL_Absolute);
	}
}

# pragma endregion

//Acess the online interface and call create session
void UMyNetWorkComponent::CreateGameSession()
{
	//Call when pressing the 1 key
	if (!onlineSessionInterface.IsValid())
	{
		return;
	}
	
	// To avoid having one session created on top of each other check if the session already exists if so delete before create a new one.
	auto ExistingSession =  onlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		onlineSessionInterface->DestroySession(NAME_GameSession);
	}

	//If there is no session, create one***
	//Before create game session add the delegate to the sessionInterface delegate list
	onlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	//Set up the session
	sessionSettings->bIsLANMatch = false;
	sessionSettings->NumPublicConnections = 4;
	sessionSettings->bAllowJoinInProgress = true;
	sessionSettings->bAllowJoinViaPresence = true;
	sessionSettings->bShouldAdvertise = true;
	sessionSettings->bUsesPresence = true;
	sessionSettings->bUseLobbiesIfAvailable = true;
	sessionSettings->Set(FName("MatchType"),FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//in Unreal Engine, the function GetPreferredUniqueNetId() is used to retrieve the unique network identifier (NetId) for a local player. It is a member function of the ULocalPlayer class, which represents a player who is locally controlled on the client.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	onlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionSettings);

}

//Joing game Sessions
void UMyNetWorkComponent::JoinGameSession()
{
	//Safely check pointers before accessing them.
	if (sessionSearch == nullptr || !onlineSessionInterface.IsValid())
	{
		return;
	}

	//Add the FindSessionDelegate to the delegate list
	onlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegate);

	//Setup the session search
	sessionSearch->MaxSearchResults = 10000;
	sessionSearch->bIsLanQuery = false;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	onlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), sessionSearch.ToSharedRef());

}

void UMyNetWorkComponent::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//This function will be called when the delegate bound to this is fired and if its parameters will be filled up with the proper data.

	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15,
				FColor::Green,
				FString::Printf(TEXT("Created section: %s"), *SessionName.ToString()));
		}

		//Once the session was created travel to the lobby onpen it as a listen server.
		UWorld* World = GetWorld();

		if (World)
		{
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
							     	15,
									FColor::Red,
							TEXT("Conection failed..."));
		}
	}
}

//if a session is found then this function will be called and bWasSuccessful will be true.
void UMyNetWorkComponent::OnFindSessionComplete(bool bWasSuccessful)
{

	if (!onlineSessionInterface.IsValid())
	{
		return;
	}

	//FOnlineSession search contains a variable called searchResults it is an array of all sessions found.
	for (auto Result : sessionSearch->SearchResults )
	{
		//Now we can get informations about the session found if there is any
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString FoundMatchType;//Goes into session settings and return with the value correspondent to the type found

		//check the result for session type
		Result.Session.SessionSettings.Get(FName("MatchType"), FoundMatchType);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15,
				FColor::Yellow,
				FString::Printf(TEXT("ID: %s -- User Name: %s"),*Id, *User));
		}

		//If the type was found print it
		if (FoundMatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,
					15,
					FColor::Yellow,
					FString::Printf(TEXT("Match Type: %s"),*FoundMatchType));
			}

			//add joinDelegate to the delegate list // now the call back function can be called after join session has been completed
			onlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);

			//Getting the local player netId
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

			//Call Join session
			onlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession, Result);
		}
	}
}

void UMyNetWorkComponent::OnjoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!onlineSessionInterface.IsValid())
	{
		return;
	}

	FString Address;
	if (onlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15,
				FColor::Yellow,
				FString::Printf(TEXT("Connect string: %s"), *Address));
		}
	}

	//Now we have the IP to connect to, we can join the session
	APlayerController* playerController = GetOwner()->GetGameInstance()->GetFirstLocalPlayerController();
	if (playerController)
	{
		playerController->ClientTravel(Address, TRAVEL_Absolute);
	}	
}