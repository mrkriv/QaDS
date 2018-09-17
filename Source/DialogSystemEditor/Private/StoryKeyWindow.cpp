// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "StoryKeyWindow.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "StoryInformationManager.h"
#include "FileManager.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SStoryKeyWindow::Construct(const FArguments& InArgs)
{
	keyManager = UStoryKeyManager::GetStoryKeyManager();

	typeNames.Add(MakeShareable(new FString("General")));
	typeNames.Add(MakeShareable(new FString("DialogPhrases")));
	typeNames.Add(MakeShareable(new FString("Task")));

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.Padding(4.0f, 2.0f, 4.0f, 2.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Import"))
						.OnClicked(this, &SStoryKeyWindow::HandleImportButton)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Export"))
						.OnClicked(this, &SStoryKeyWindow::HandleImportButton)
					]
				]
				+ SVerticalBox::Slot()
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
					  	SAssignNew(findTextBox, SEditableTextBox)
						.HintText(FText::FromString("Find key"))
					]
				]
				+ SVerticalBox::Slot()
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				[
					SNew(SHorizontalBox)
				]
				+ SVerticalBox::Slot()
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
					  	SAssignNew(editKeyTextBox, SEditableTextBox)
						.HintText(FText::FromString("Enter key name"))
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Add"))
						.OnClicked(this, &SStoryKeyWindow::HandleAddKeyButton)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Remove"))
						.OnClicked(this, &SStoryKeyWindow::HandleRemoveKeyButton)
					]
				]
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SStoryKeyWindow::HandleImportButton()
{
	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(FText::FromString("Import successful")));
	return FReply::Handled();
}

FReply SStoryKeyWindow::HandleExportButton()
{
	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(FText::FromString("Export successful")));
	return FReply::Handled();
}

FReply SStoryKeyWindow::HandleAddKeyButton()
{
	auto key = editKeyTextBox->GetText().ToString();
	if (keyManager->AddKey(*key))
	{
		FSlateNotificationManager::Get().AddNotification(FNotificationInfo(FText::FromString("Add " + key)));
	}

	return FReply::Handled();
}

FReply SStoryKeyWindow::HandleRemoveKeyButton()
{
	auto key = editKeyTextBox->GetText().ToString();
	if (keyManager->RemoveKey(*key))
	{
		FSlateNotificationManager::Get().AddNotification(FNotificationInfo(FText::FromString("Remove " + key)));
	}

	return FReply::Handled();
}
