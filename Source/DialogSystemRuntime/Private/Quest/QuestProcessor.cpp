#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "QuestAsset.h"
#include "QuestNode.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"

UQuestProcessor* UQuestProcessor::GetQuestProcessor()
{
	for (TObjectIterator<UQuestProcessor> Itr; Itr;)
		return *Itr;

	return NewObject<UQuestProcessor>();
}

void UQuestProcessor::StartQuest(TAssetPtr<UQuestAsset> QuestAsset)
{
	if (StoryKeyManager == NULL)
	{
		StoryKeyManager = UStoryKeyManager::GetStoryKeyManager();
	}

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

	quest->RootNode->Prepare(this, quest);
	auto stages = quest->RootNode->GetNextStage();
	if (stages.Num() == 0)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: not found valid stage in %s"), *QuestAsset.GetAssetName());
		return;
	}

	activeQuests.Add(quest);
	OnQuestStart.Broadcast(quest);
	OnQuestStartBP.Broadcast(quest);

	WaitStage(quest->RootNode);
}

void UQuestProcessor::WaitStage(UQuestNode* StageNode)
{
	check(StageNode);
	check(StageNode->OwnerQuest);

	auto stages = StageNode->GetNextStage();

	if (stages.Num() == 0)
	{
		EndQuest(StageNode->OwnerQuest, StageNode->Status == EQuestCompleteStatus::Completed);
		return;
	}

	for (auto stage : stages)
	{
		stage->Activate();

		if (stage->TryComplete())
		{
			if (StageNode->OwnerQuest->Status != EQuestCompleteStatus::Active)
				break;
		}
	}
}

void UQuestProcessor::CompleteStage(UQuestNode* StageNode)
{
	check(StageNode);
	check(StageNode->OwnerQuest);

	OnQuestStageChange.Broadcast(StageNode->OwnerQuest, StageNode->Stage);
	OnQuestStageChangeBP.Broadcast(StageNode->OwnerQuest, StageNode->Stage);

	WaitStage(StageNode);
}

void UQuestProcessor::EndQuest(UQuestAsset* Quest, bool IsSuccses)
{
	if (!activeQuests.Remove(Quest))
	{
		UE_LOG(DialogModuleLog, Warning, TEXT("Failed end quest: quest is not active (%s)"), *Quest->GetFName().ToString());
		return;
	}

	if (Quest->Status == EQuestCompleteStatus::Active)
	{
		Quest->Status = IsSuccses ? EQuestCompleteStatus::Completed : EQuestCompleteStatus::Failed;
	}

	archiveQuests.Add(Quest);

	OnQuestEnd.Broadcast(Quest, IsSuccses);
	OnQuestEndBP.Broadcast(Quest, IsSuccses);
}

TArray<UQuestAsset*> UQuestProcessor::GetActiveQuests() const
{
	return activeQuests;
}

TArray<UQuestAsset*> UQuestProcessor::GetArchiveQuests() const
{
	return archiveQuests;
}