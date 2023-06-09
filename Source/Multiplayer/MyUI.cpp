// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUI.h"
#include "SlateOptMacros.h"

void MyUI::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("Ex", "HelloWorld", "Hello World!"))
		];
}
