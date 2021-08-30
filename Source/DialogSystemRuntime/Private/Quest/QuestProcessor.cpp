#include "DialogSystemRuntime.h"
#include "QuestAsset.h"
#include "QuestNode.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"
#include "QaDSSettings.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"

UQuestProcessor* UQuestProcessor::Instance = NULL;

UQuestProcessor* UQuestProcessor::GetQuestProcessor(UObject* WorldContextObject)
{
	if (Instance == NULL)
		Instance = NewObject<UQuestProcessor>(WorldContextObject);

	if (Instance->StoryKeyManager == NULL)
		Instance->StoryKeyManager = UStoryKeyManager::GetStoryKeyManager(WorldContextObject);

	if (Instance->StoryTriggerManager == NULL)
		Instance->StoryTriggerManager = UStoryTriggerManager::GetStoryTriggerManager(WorldContextObject);

	return Instance;
}

void UQuestProcessor::BeginDestroy()
{
	Super::BeginDestroy();

	if (Instance == this)
		Instance = NULL;
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
	runtimeQuest->Asset = quest;
	runtimeQuest->CreateScript();

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
		isOptionalOnly &= stage->Stage.bIsOptional;
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

	auto isNeedEvents = StageNode->Stage.bGenerateEvents;
	isNeedEvents = !StageNode->Stage.Caption.IsEmpty();

	auto isEmpty = StageNode->Stage.Caption.IsEmpty();
	auto generateForEmpty = !GetDefault<UQaDSSettings>()->bDontGenerateEventForEmptyQuestNode;
	
	if (generateForEmpty && StageNode->Stage.bGenerateEvents || !generateForEmpty && !isEmpty)
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

	if (GetDefault<UQaDSSettings>()->bUseQuestArchive)
	{
		archiveQuests.Add(Quest);
	}

	OnQuestEnd.Broadcast(Quest, Status);
	Quest->DestroyScript();
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

		quest->DestroyScript();
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
		if (GetDefault<UQaDSSettings>()->bUseQuestArchive)
		{
			for (auto quest : A.archiveQuests)
			{
				archiveQuestsArchive.Add(quest);
			}
		}

		for (auto quest : A.activeQuests)
		{
			activeQuestsArchive.Add(quest);
		}
	}

	Ar << archiveQuestsArchive << activeQuestsArchive;
	//todo:: save QuestScript

	if (Ar.IsLoading())
	{
		A.archiveQuests.Reset();
		A.activeQuests.Reset();

		if (GetDefault<UQaDSSettings>()->bUseQuestArchive)
		{
			for (auto& archive : archiveQuestsArchive)
			{
				A.archiveQuests.Add(archive.Load());
			}
		}

		for (auto& active : activeQuestsArchive)
		{
			auto quest = active.Load();
			quest->CreateScript();
			A.activeQuests.Add(quest);
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