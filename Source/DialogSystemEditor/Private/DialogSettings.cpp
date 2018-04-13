// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "DialogSettings.h"

UDialogSettings::UDialogSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AutoCompile = true;
	MemorizeReadingPhrase = true;
	NodeButtonDefault = FLinearColor(0.9f, 0.9f, 0.9f);
	NodeButtonHovered = FLinearColor(0.02f, 0.02f, 0.02f);
	NodeRoot = FLinearColor(0.0f, 0.4f, 1.0f);
	NodePlayer = FLinearColor(1.0f, 0.4f, 0.0f);
	FontSize = 14;
}