// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/UI/SFirstMenuWidget.h"

#define LOCTEXT_NAMESPACE "MuliplayerMenu"

void SFirstMenuWidget::Construct(const FArguments& InArgs)
{

	const FMargin ContentPadding = FMargin(500.0f, 300.0f);
	const FMargin ButtonPadding = FMargin(10.0f);

	const FText TitleText = LOCTEXT("Multiplayer", "Multiplayer Window");
	const FText OpenLobbyText = LOCTEXT("OpenLobby", "OpenLobby");
	const FText JoinLobbyText = LOCTEXT("JoinLobby", "JoinLobby");

	FSlateFontInfo ButtonTextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");
	ButtonTextStyle.Size = 40.0f;

	FSlateFontInfo TitleTextStyle = ButtonTextStyle;
	TitleTextStyle.Size = 60.0f;

	ChildSlot
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.ColorAndOpacity(FColor::Black)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(ContentPadding)
			[
				SNew(SVerticalBox)
				//Text Title
				+SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Font(TitleTextStyle)
					.Text(TitleText)
					.Justification(ETextJustify::Center)
				]
				//Button OpenLobby
				+ SVerticalBox::Slot()
				.Padding(ButtonPadding)
				[
					SNew(SButton)
					.OnClicked(this, &SFirstMenuWidget::OnCreateClicked)
					[
						SNew(STextBlock)
						.Font(ButtonTextStyle)
						.Text(OpenLobbyText)
						.Justification(ETextJustify::Center)
					]
				]
				//Button JoinLobby
				+ SVerticalBox::Slot()
				.Padding(ButtonPadding)
				[
					SNew(SButton)
					.OnClicked(this, &SFirstMenuWidget::OnJoinClicked)
					[
						SNew(STextBlock)
						.Font(ButtonTextStyle)
						.Text(JoinLobbyText)
						.Justification(ETextJustify::Center)
					]
				]
			]
		];
}

FReply SFirstMenuWidget::OnCreateClicked() const
{
	return FReply::Handled();
}

FReply SFirstMenuWidget::OnJoinClicked() const
{
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
