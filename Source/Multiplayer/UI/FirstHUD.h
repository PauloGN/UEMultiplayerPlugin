// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FirstHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AFirstHUD : public AHUD
{
	GENERATED_BODY()
protected:

	TSharedPtr<class SFirstMenuWidget> MenuWidget;
	TSharedPtr<class SWidget> MenuWidgetContainer;

	virtual void BeginPlay() override;
};
