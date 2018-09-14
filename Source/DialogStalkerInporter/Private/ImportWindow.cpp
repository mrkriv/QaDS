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

#include "DialogAssetFactory.h"
#include "DialogAsset.h"
#include "Engine/Engine.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "UnrealEd.h"

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
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.2f)
					[
					  	SNew(STextBlock)
						.Text(FText::FromString("Gamedata path"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.8f)
					[
					  	SAssignNew(gamedataPathTextBox, SEditableTextBox)
						.Text(FText::FromString("D:\\Program Files\\Dead Air\\database\\unpacked"))
					]
				]
				+ SVerticalBox::Slot()
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(0.2f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Output path"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.8f)
					[
					  	SAssignNew(outputPathTextBox, SEditableTextBox)
						.Text(FText::FromString("/Game//Dialogs"))
					]
				]
				+ SVerticalBox::Slot()
				.Padding(4.0f, 2.0f, 4.0f, 2.0f)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Right)
					[
						SNew(SButton)
						.Text(FText::FromString("Import"))
						.OnClicked(this, &SImportWindow::HandleImportButton)
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

FReply SImportWindow::HandleImportButton()
{
	auto basePath = gamedataPathTextBox->GetText().ToString();
	auto gameplayPath = basePath / "configs" / "gameplay";
	auto dialogs = GetFilesInDir(gameplayPath, "dialogs*.xml");

	for (auto dialogFile : dialogs)
	{
		FXmlFile file(gameplayPath / dialogFile);
		auto root = file.GetRootNode();

		if (root == NULL)
			continue;

		for (auto node : root->GetChildrenNodes())
		{
			if (node->GetTag() != "dialog")
				continue;

			auto asset = CreateDialogAsset(dialogFile, node->GetAttribute("id"));
			ImportNodes(asset, node);

			UE_LOG(DialogStalkerInporterLog, Log, TEXT("Create dialog asset %s"), *asset->GetFullName());

			return FReply::Handled();
		}
	}

	return FReply::Handled();
}

UDialogAsset* SImportWindow::CreateDialogAsset(const FString& path, const FString& name)
{
	auto& assetTools = FAssetToolsModule::GetModule().Get();
	auto factory = NewObject<UDialogAssetFactory>();

	auto packageName = outputPathTextBox->GetText().ToString() / path;
	auto newAsset = assetTools.CreateAsset(name, packageName, factory->GetSupportedClass(), factory);
	
	TArray<UObject*> ObjectsToSync;
	ObjectsToSync.Add(newAsset);
	GEditor->SyncBrowserToObjects(ObjectsToSync);

	return Cast<UDialogAsset>(newAsset);
}

void SImportWindow::ImportNodes(UDialogAsset* asset, FXmlNode* dialogNode)
{
	auto xmlPhraseList = dialogNode->FindChildNode("phrase_list");

	if (xmlPhraseList == NULL)
		return;

	TMap<FString, UDialogPhrase*> phrases;
	for (auto xmlPhrase : xmlPhraseList->GetChildrenNodes())
	{
		auto phrase = NewObject<UDialogPhrase>();
		phrase->OwnerDialog = asset;
		phrase->Data.UID = *xmlPhrase->GetAttribute("id");
		phrase->Data.Text = FText::FromString(xmlPhrase->FindChildNode("text")->GetContent());

		phrases.Add(phrase->Data.UID.ToString(), phrase);
	}

	for (auto xmlPhrase : xmlPhraseList->GetChildrenNodes())
	{
		UDialogPhrase* phrase = phrases[xmlPhrase->GetAttribute("id")];

		for (auto node : xmlPhrase->GetChildrenNodes())
		{
			auto tag = node->GetTag();
			auto content = node->GetContent();
			if (tag == "next")
			{
				phrase->Childs.AddUnique(phrases[content]);
			}
			else if (tag == "has_info")
			{
				phrase->Data.CheckHasKeys.Add(*content);
			}
			else if (tag == "dont_has_info")
			{
				phrase->Data.CheckDontHasKeys.Add(*content);
			}
			else if (tag == "give_info")
			{
				phrase->Data.GiveKeys.Add(*content);
			}
			else if (tag == "disable_info")
			{
				phrase->Data.RemoveKeys.Add(*content);
			}
		}
	}

	asset->RootNode = NewObject<UDialogPhrase>();
	asset->RootNode->OwnerDialog = asset;
	asset->RootNode->Data.UID = "0";
	asset->RootNode->Childs.Add(phrases["0"]);

	TArray<UDialogPhrase*> handled;

	for (auto child : asset->RootNode->Childs)
		SetNodeSource(handled, Cast<UDialogPhrase>(child), false);
}

void SImportWindow::SetNodeSource(TArray<UDialogPhrase*>& handled, UDialogPhrase* phrase, bool isParentActor)
{
	if (phrase == NULL || handled.Contains(phrase))
		return;

	handled.Add(phrase);
	phrase->Data.Source = isParentActor ? EDialogPhraseSource::Player : EDialogPhraseSource::NPC;

	for (auto child : phrase->Childs)
		SetNodeSource(handled, phrase, !isParentActor);
}