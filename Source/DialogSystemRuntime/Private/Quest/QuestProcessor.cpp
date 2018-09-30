#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "QuestAsset.h"
#include "QuestNode.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"

UQuestProcessor* UQuestProcessor::GetQuestProcessor()
{
	TObjectIterator<UQuestProcessor> iter;
	auto result = iter ? *iter : NewObject<UQuestProcessor>();

	if (result->StoryKeyManager == NULL)
		result->StoryKeyManager = UStoryKeyManager::GetStoryKeyManager();

	if (result->StoryTriggerManager == NULL)
		result->StoryTriggerManager = UStoryTriggerManager::GetStoryTriggerManager();

	return result;
}

void UQuestProcessor::StartQuest(TAssetPtr<UQuestAsset> QuestAsset)
{
	auto quest = QuestAsset.LoadSynchronous();
	if (quest == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: asset is not set"));
		return;
	}

	if (quest->RootNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: not found root node in %s"), *QuestAsset.GetAssetName());
		return;
	}

	auto runtimeQuest = quest->Load(this);
	runtimeQuest->Status = EQuestCompleteStatus::Active;

	auto stages = runtimeQuest->RootNode->GetNextStage();
	if (stages.Num() == 0)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed start new quest: not found valid stage in %s"), *QuestAsset.GetAssetName());
		return;
	}
	
	activeQuests.Add(runtimeQuest);
	OnQuestStart.Broadcast(runtimeQuest);

	WaitStage(runtimeQuest->RootNode);
}

void UQuestProcessor::WaitStage(UQuestRuntimeNode* StageNode)
{
	if (bIsResetBegin)
		return;

	check(StageNode);
	check(StageNode->OwnerQuest);

	auto stages = StageNode->GetNextStage();

	if (stages.Num() == 0)
	{
		EndQuest(StageNode->OwnerQuest, EQuestCompleteStatus::Completed);
		return;
	}

	for (auto stage : stages)
	{
		stage->SetStatus(EQuestCompleteStatus::Active);

		if (stage->TryComplete())
		{
			if (StageNode->OwnerQuest->Status != EQuestCompleteStatus::Active)
				break;
		}
	}
}

void UQuestProcessor::CompleteStage(UQuestRuntimeNode* StageNode)
{
	if (bIsResetBegin)
		return;

	check(StageNode);
	check(StageNode->OwnerQuest);

	OnStageComplete.Broadcast(StageNode->OwnerQuest, StageNode->Stage);

	WaitStage(StageNode);
}

void UQuestProcessor::EndQuest(UQuestRuntimeAsset* Quest, EQuestCompleteStatus Status)
{
	if (bIsResetBegin)
		return;

	if (!activeQuests.Remove(Quest))
	{
		UE_LOG(DialogModuleLog, Warning, TEXT("Failed end quest: quest is not active (%s)"), *Quest->GetFName().ToString());
		return;
	}

	if (Quest->Status == EQuestCompleteStatus::Active)
	{
		Quest->Status = Status;
	}

	archiveQuests.Add(Quest);

	OnQuestEnd.Broadcast(Quest, Status);
}

TArray<UQuestRuntimeAsset*> UQuestProcessor::GetQuests(EQuestCompleteStatus FilterStatus) const
{
	if (FilterStatus == EQuestCompleteStatus::None)
	{
		auto result = archiveQuests;
		result.Append(activeQuests);

		return result;
	}
	if (FilterStatus == EQuestCompleteStatus::Active)
	{
		return activeQuests;
	}

	return archiveQuests.FilterByPredicate([FilterStatus](UQuestRuntimeAsset* quest)
	{
		return quest->Status == FilterStatus;
	});
}

void UQuestProcessor::Reset()
{
	bIsResetBegin = true;

	for (auto quest : activeQuests)
	{
		for (auto stage : quest->ActiveNodes)
		{
			stage->SetStatus(EQuestCompleteStatus::Skiped);
		}
	}

	archiveQuests.Reset();
	activeQuests.Reset();

	bIsResetBegin = false;
}