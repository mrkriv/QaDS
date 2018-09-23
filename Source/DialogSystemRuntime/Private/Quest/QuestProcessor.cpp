#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "QuestAsset.h"
#include "QuestNodes.h"
#include "QuestProcessor.h"

UQuestProcessor* UQuestProcessor::GetQuestProcessor()
{
	for (TObjectIterator<UQuestProcessor> Itr; Itr;)
		return *Itr;

	return NewObject<UQuestProcessor>();
}

TArray<UQuestAsset*> UQuestProcessor::GetActiveQuests() const
{
	return activeQuests;
}

void UQuestProcessor::StartQuest(TAssetPtr<UQuestAsset> QuestAsset)
{
	if (!QuestAsset.IsValid())
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: asset is not set"));
		return;
	}

	auto quest = QuestAsset.Get();

	if (quest->RootNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: not found root node in %s"), *QuestAsset.GetAssetName());
		return;
	}

	auto stages = quest->RootNode->FindNextStage();

	activeQuests.Add(quest);

	OnQuestStart.Broadcast(quest);
	OnQuestStartBP.Broadcast(quest);
}

void UQuestProcessor::EndQuest(UQuestAsset* Quest, bool IsSuccses)
{
	if (!activeQuests.Remove(Quest))
		return;

	OnQuestEnd.Broadcast(Quest, IsSuccses);
	OnQuestEndBP.Broadcast(Quest, IsSuccses);
}