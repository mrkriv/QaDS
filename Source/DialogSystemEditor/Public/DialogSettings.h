// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "UObject/NoExportTypes.h"
#include "DialogSettings.generated.h"


UCLASS(config = Engine, defaultconfig)
class DIALOGSYSTEMEDITOR_API UDialogSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = Settings)
	bool AutoCompile = true;
};