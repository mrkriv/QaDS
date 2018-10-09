#pragma once

#include "UObject/NoExportTypes.h"
#include "QaDSSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class DIALOGSYSTEMRUNTIME_API UQaDSSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = Settings)
	bool AutoCompile = true;

	UPROPERTY(config, EditAnywhere, Category = Quest)
	bool bDontGenerateEventForEmptyQuestNode = true;

	UPROPERTY(config, EditAnywhere, Category = Quest)
	bool bUseQuestArchive = true;
};