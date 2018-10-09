#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "QuestAsset.h"
#include "QuestNode.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"

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

	if (quest->bIsSingltone)
	{
		for (auto activeQuest : activeQuests)
		{
			if (activeQuest->Asset->GetFullName() == quest->GetFullName())
			{
				UE_LOG(DialogModuleLog, Log, TEXT("Quest %s already active"), *QuestAsset.GetAssetName());
				return;
			}
		}
	}
	
	auto runtimeQuest = NewObject<UQuestRuntimeAsset>();
	runtimeQuest->Status = EQuestCompleteStatus::Active;
	// todo:: create runtime->QuestScript
	runtimeQuest->Asset = quest;

	activeQuests.Add(runtimeQuest);
	OnQuestStart.Broadcast(runtimeQuest);

	auto root = runtimeQuest->LoadNode(quest->RootNode);
	WaitStage(root);
}

void UQuestProcessor::WaitStage(UQuestRuntimeNode* StageNode)
{
	if (bIsResetBegin)
		return;

	if (StageNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed jump to null quest stage"));
		return;
	}

	check(StageNode->OwnerQuest);

	auto stages = StageNode->GetNextStage();
	auto isOptionalOnly = true;

	for (auto stage : stages)
	{
		stage->Stage.bIsOptional &= isOptionalOnly;
	}

	if (stages.Num() == 0 || isOptionalOnly)
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

	if(!activeQuests.Contains(StageNode->OwnerQuest))
		return;

	if (StageNode->Stage.bGenerateEvents) //&& !StageNode->Stage.Caption.IsEmpty() /*|| Setting.bGenerateEventForEmptyQuestNode */) // todo:: add setting
	{
		OnStageComplete.Broadcast(StageNode->OwnerQuest, StageNode->Stage);
	}

	if(StageNode->Status == EQuestCompleteStatus::Completed)
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

FArchive& operator<<(FArchive& Ar, UQuestProcessor& A)
{
	TArray<FQuestRuntimeAssetArchive> archiveQuestsArchive;
	TArray<FQuestRuntimeAssetArchive> activeQuestsArchive;

	if (Ar.IsSaving())
	{
		for (auto quest : A.archiveQuests)
		{
			archiveQuestsArchive.Add(quest);
		}

		for (auto quest : A.activeQuests)
		{
			activeQuestsArchive.Add(quest);
		}
	}

	Ar << archiveQuestsArchive << activeQuestsArchive;

	if (Ar.IsLoading())
	{
		A.archiveQuests.Reset();
		A.activeQuests.Reset();

		for (auto& archive : archiveQuestsArchive)
		{
			A.archiveQuests.Add(archive.Load());
		}

		for (auto& archive : activeQuestsArchive)
		{
			A.activeQuests.Add(archive.Load());
		}
	}

	return Ar;
}

TArray<uint8> UQuestProcessor::SaveToBinary()
{
	TArray<uint8> result;
	FMemoryWriter writter(result);
	writter << *this;

	return result;
}

void UQuestProcessor::LoadFromBinary(const TArray<uint8>& Data)
{
	FMemoryReader reader(Data);
	reader << *this;
}