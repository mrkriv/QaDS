// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "ImportWindow.h"
#include "DialogStalkerInporterModule.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "FileManager.h"
#include "XmlFile.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SImportWindow::Construct(const FArguments& InArgs)
{
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
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					[
						SAssignNew(gamedataPathTextBox, SEditableTextBox)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Import"))
						.OnClicked(this, &SImportWindow::HandleScanButton)
					]
				]
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TArray<FString> SImportWindow::GetFilesInDir(FString path, FString mask)
{
	FPaths::NormalizeDirectoryName(path);
	path /= mask;

	TArray<FString> files;
	IFileManager::Get().FindFiles(files, *path, true, false);

	return files;
}

FReply SImportWindow::HandleScanButton()
{
	auto basePath = gamedataPathTextBox->GetText().ToString();
	auto gameplayPath = basePath / "configs" / "gameplay";
	auto dialogs = GetFilesInDir(gameplayPath, "dialogs*.xml");

	for (auto dialogFile : dialogs)
	{
		UE_LOG(DialogStalkerInporterLog, Log, TEXT("Dialog file: (%s)"), *dialogFile);

		FXmlFile file(gameplayPath / dialogFile);
		auto root = file.GetRootNode();

		if (root == NULL)
			continue;

		for (auto node : root->GetChildrenNodes())
		{
			if (node->GetTag() != "dialog")
				continue;

			auto id = node->GetAttribute("id");

			UE_LOG(DialogStalkerInporterLog, Log, TEXT("Dialog found: (%s)"), *id);
		}
	}

	return FReply::Handled();
}

FReply SImportWindow::HandleImportButton()
{
	return FReply::Handled();
}
