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
						.Text(FText::FromString("D:\\gamedata\\"))
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
						.Text(FText::FromString("Local name"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.8f)
					[
					  	SAssignNew(localNameTextBox, SEditableTextBox)
						.Text(FText::FromString("rus"))
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
						.Text(FText::FromString("Dialog ID mask"))
					]
					+ SHorizontalBox::Slot()
					.FillWidth(0.8f)
					[
					  	SAssignNew(dialogIdMaskTextBox, SEditableTextBox)
						.Text(FText::FromString(""))
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

	LoadLocalize(basePath / "configs" / "text" / localNameTextBox->GetText().ToString());

	auto dialogs = GetFilesInDir(gameplayPath, "dialogs*.xml");
	auto idMask = dialogIdMaskTextBox->GetText().ToString();

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

			auto id = node->GetAttribute("id");
			if (!idMask.IsEmpty() && id.Contains(idMask))
				continue;

			auto asset = CreateDialogAsset(dialogFile, id);
			ImportNodes(asset, node);

			UE_LOG(DialogStalkerInporterLog, Log, TEXT("Create dialog asset %s"), *asset->GetFullName());
		}
	}

	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(FText::FromString("Import successful")));
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

	TMap<FString, UDialogPhraseNode*> phrases;
	for (auto xmlPhrase : xmlPhraseList->GetChildrenNodes())
	{
		auto id = xmlPhrase->GetAttribute("id");
		auto text = xmlPhrase->FindChildNode("text")->GetContent();

		auto phrase = NewObject<UDialogPhraseNode>();
		phrase->OwnerDialog = asset;
		phrase->Data.UID = *id;
		phrase->Data.Text = GetLocalizeString(text);

		phrases.Add(phrase->Data.UID.ToString(), phrase);
	}

	for (auto xmlPhrase : xmlPhraseList->GetChildrenNodes())
	{
		UDialogPhraseNode* phrase = phrases[xmlPhrase->GetAttribute("id")];

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

	asset->RootNode = NewObject<UDialogNode>();
	asset->RootNode->OwnerDialog = asset;
	asset->RootNode->Childs.Add(phrases["0"]);

	TArray<UDialogPhraseNode*> handled;

	for (auto child : asset->RootNode->Childs)
		SetNodeSource(handled, Cast<UDialogPhraseNode>(child), true);
}

void SImportWindow::SetNodeSource(TArray<UDialogPhraseNode*>& handled, UDialogPhraseNode* phrase, bool isParentActor)
{
	if (phrase == NULL || handled.Contains(phrase))
		return;

	handled.Add(phrase);
	phrase->Data.Source = isParentActor ? EDialogPhraseSource::Player : EDialogPhraseSource::NPC;

	for (auto child : phrase->Childs)
		SetNodeSource(handled, Cast<UDialogPhraseNode>(child), !isParentActor);
}

void SImportWindow::LoadLocalize(FString path)
{
	auto xmls = GetFilesInDir(path, "*.xml");

	for (auto file : xmls)
	{
		FXmlFile file(path / file);
		auto root = file.GetRootNode();

		if (root == NULL)
			continue;

		for (auto node : root->GetChildrenNodes())
		{
			auto id = node->GetAttribute("id");
			auto textNode = node->FindChildNode("text");

			if (textNode != NULL)
			{
				localize.Add(id, FText::FromString(textNode->GetContent()));
			}
		}
	}
}

FText SImportWindow::GetLocalizeString(FString keyName)
{
	auto ptr = localize.Find(keyName);

	if (ptr != NULL)
		return *ptr;

	return FText::FromString(keyName);
}
