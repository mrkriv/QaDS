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

	FReply HandleImportButton();
	FReply HandleExportButton();
	FReply HandleAddKeyButton();
	FReply HandleRemoveKeyButton();

private:
	TSharedPtr<SEditableTextBox> findTextBox;
	TSharedPtr<STextComboBox> findTypeComboBox;
	TSharedPtr<SEditableTextBox> editKeyTextBox;
	TSharedPtr<STextComboBox> editTypeComboBox;

	TArray<TSharedPtr<FString>> typeNames;
	class UStoryKeyManager* keyManager;
};