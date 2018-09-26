#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "QuestAsset.h"
#include "QuestNode.h"
#include "QuestProcessor.h"

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

	auto stages = quest->RootNode->GetNextStage(this);
	if (stages.Num() == 0)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: not found valid stage in %s"), *QuestAsset.GetAssetName());
		return;
	}

	activeQuests.Add(quest);
	OnQuestStart.Broadcast(quest);
	OnQuestStartBP.Broadcast(quest);

	quest->RootNode->SetOwnerQuest(quest);
	WaitStage(quest->RootNode);
}

void UQuestProcessor::WaitStage(UQuestNode* root)
{
	auto stages = root->GetNextStage(this);
	for (auto stage : stages)
	{
		if (!activeQuests.Contains(stage->OwnerQuest)) //todo:: add Status ti UQuestAsset and use hear
			break;

		root->OwnerQuest->ActiveNodes.Add(stage);
		if (!stage->TryComplete(this))
		{
			stage->Assign(this);
		}
	}
}

void UQuestProcessor::ActivateStage(UQuestNode* StageNode)
{
	check(StageNode);
	check(StageNode->OwnerQuest);

	auto quest = StageNode->OwnerQuest;
	if (!quest->ActiveNodes.Remove(StageNode))
	{
		UE_LOG(DialogModuleLog, Warning, TEXT("Failed activate quest stage: stage is not active (%s)"), *quest->GetFName().ToString());
		return;
	}

	//quest->CompletedNodes.Add(StageNode); //todo:: use status

	StageNode->InvokePostScript(this);
	OnQuestStageChange.Broadcast(quest, StageNode->Stage);
	OnQuestStageChangeBP.Broadcast(quest, StageNode->Stage);

	WaitStage(StageNode);
}

void UQuestProcessor::EndQuest(UQuestAsset* Quest, bool IsSuccses)
{
	if (!activeQuests.Remove(Quest))
	{
		UE_LOG(DialogModuleLog, Warning, TEXT("Failed end quest: quest is not active (%s)"), *Quest->GetFName().ToString());
		return;
	}

	if (IsSuccses)
	{
//		completedQuests.Add(Quest);
	}
	else
	{
		//failedQuests.Add(Quest);
	}

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