// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SStoryKeyWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStoryKeyWindow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void UpdateKeys();

	FReply HandleImportButton();
	FReply HandleExportButton();
	FReply HandleAddKeyButton();
	FReply HandleRemoveKeyButton();
	void HandleSelectKey(TSharedPtr<FName> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<ITableRow> HandleGenerateRow(TSharedPtr<FName> Item, const TSharedRef<STableViewBase>& OwnerTable);

private:
	class UStoryKeyManager* keyManager;
	TArray<TSharedPtr<FName>> keys;

	TSharedPtr<SSearchBox> searchBox;
	TSharedPtr<SEditableTextBox> editKeyTextBox;
	TSharedPtr<SListView<TSharedPtr<FName>>> keyListView;

	void LogInfo(const FString& message);
};