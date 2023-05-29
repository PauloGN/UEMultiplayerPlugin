// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
//OnlineSessions
#include "Interfaces/OnlineSessionInterface.h"
//OnlineSessions End
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UMultiplayerSessionsSubsystem();

protected:

private:

	class IOnlineSubsystem* subsystem;
	IOnlineSessionPtr sessionInterface;

};
