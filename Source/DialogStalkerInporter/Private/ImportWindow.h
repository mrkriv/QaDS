// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SImportWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SImportWindow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply HandleScanButton();
	FReply HandleImportButton();

private:
	TSharedPtr<class SEditableTextBox> gamedataPathTextBox;
};