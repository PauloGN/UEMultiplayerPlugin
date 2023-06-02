// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
/**
 * 
 */
class SFirstMenuWidget : SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFirstMenuWidget) { }

	SLATE_ARGUMENT(TWeakObjectPtr<class AFirstHUD>, OwningHUD)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply OnCreateClicked() const;
	FReply OnJoinClicked() const;

	TWeakObjectPtr<class AMenuHUD> OwningHUD;

	virtual bool SupportsKeyboardFocus() const override { return true; };
};
