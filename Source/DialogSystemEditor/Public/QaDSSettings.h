#pragma once

#include "UObject/NoExportTypes.h"
#include "QaDSSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class DIALOGSYSTEMEDITOR_API UQaDSSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = Settings)
	bool AutoCompile = true;
};