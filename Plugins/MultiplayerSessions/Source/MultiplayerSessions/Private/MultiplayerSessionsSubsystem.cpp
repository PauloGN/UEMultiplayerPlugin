// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
//OnlineSessions
#include "OnlineSubsystem.h"//included as a dependency for our multiplayer sessions plugin
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//Iniatialize subsystem and session interface references
	subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		sessionInterface = subsystem->GetSessionInterface();
	}
}
