#pragma once

#include "QuestScript.generated.h"

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API AQuestScript : public AInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	class UQuestRuntimeAsset* Quest;
};
