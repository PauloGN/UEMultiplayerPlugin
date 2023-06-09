// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"
//#include "MyUI.h"

/**
 * 
 */
class MyUI;
UCLASS()
class MULTIPLAYERUI_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
public:
	void BeginPlay() override;
	TSharedPtr<MyUI> ExampleWidget;
};
