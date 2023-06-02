// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/UI/FirstHUD.h"
#include "SFirstMenuWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Engine/Engine.h"

void AFirstHUD::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine && GEngine->GameViewport)
	{
		//Why is this OwningHUD in SFirstMenuWidget but I can only use OwningHud here??? OwningHud doesnt even exist
		MenuWidget = SNew(SFirstMenuWidget).OwningHUD(this);
		GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(MenuWidgetContainer, SWeakWidget).PossiblyNullContent(MenuWidget.ToSharedRef()));
	}
}
