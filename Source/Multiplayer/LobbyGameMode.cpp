// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		//Printing Num of players
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Cyan, FString::Printf(TEXT("Players in Game: %d"), NumberOfPlayers));
		}

		//Getting Players name from controller
		APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
		FString PlayerName;
		if (PlayerState)
		{
			PlayerName = PlayerState->GetPlayerName();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Cyan, FString::Printf(TEXT("%s has joined the game"), *PlayerName));
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		//Printing Num of players
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Cyan, FString::Printf(TEXT("Players in Game: %d"), NumberOfPlayers - 1));
		}

		//Getting Players name from controller
		APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
		FString PlayerName;
		if (PlayerState)
		{
			PlayerName = PlayerState->GetPlayerName();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Cyan, FString::Printf(TEXT("%s has Exited the game"), *PlayerName));
			}
		}
	}
}
