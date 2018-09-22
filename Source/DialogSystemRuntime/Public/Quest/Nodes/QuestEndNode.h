#pragma once

#include "QuestAsset.h"
#include "QuestEndNode.generated.h"

class UQuestAsset;

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestEndNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UQuestAsset* OwnerQuest;
};
