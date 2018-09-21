#pragma once

#include "QuestAsset.h"
#include "QuestNode.generated.h"

class UQuestAsset;

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UQuestNode*> Childs;

	UPROPERTY()
	UQuestAsset* OwnerQuest;
};
