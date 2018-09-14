// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class UDialogPhrase;
class UDialogAsset;
class SEditableTextBox;
class FXmlNode;

class SImportWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SImportWindow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply HandleImportButton();

private:
	TMap<FString, FText> localize;

	TSharedPtr<SEditableTextBox> gamedataPathTextBox;
	TSharedPtr<SEditableTextBox> localNameTextBox;
	TSharedPtr<SEditableTextBox> outputPathTextBox;

	TArray<FString> GetFilesInDir(FString path, FString mask);
	UDialogAsset* CreateDialogAsset(const FString& path, const FString& name);
	void ImportNodes(UDialogAsset* asset, FXmlNode* dialogNode);
	void SetNodeSource(TArray<UDialogPhrase*>& handled, UDialogPhrase* phrase, bool isParentActor);
	FText GetLocalizeString(FString keyName);
	void LoadLocalize(FString path);
};