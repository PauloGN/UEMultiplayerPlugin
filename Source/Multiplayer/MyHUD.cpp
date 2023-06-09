// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "MyUI.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	ExampleWidget = SNew(MyUI);

	UGameViewportClient* ViewportClient
		= GetWorld()->GetGameViewport();

	ViewportClient->AddViewportWidgetContent(
	ExampleWidget.ToSharedRef()
	);
}
