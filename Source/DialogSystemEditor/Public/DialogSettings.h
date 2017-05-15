// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "UObject/NoExportTypes.h"
#include "DialogSettings.generated.h"


UCLASS(config = Engine, defaultconfig)
class DIALOGSYSTEMEDITOR_API UDialogSettings : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(config, EditAnywhere, Category = Settings)
	bool AutoCompile;

	UPROPERTY(config, EditAnywhere, Category = Settings)
	bool MemorizeReadingPhrase;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeButtonDefault;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeButtonHovered;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeRoot;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodePlayer;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeNPC;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	int FontSize;
};