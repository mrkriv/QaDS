#include "DialogSystemRuntime.h"
#include "QuestNode.h"
#include "QuestAsset.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"

TArray<UQuestNode*> UQuestNode::GetNextStage()
{
	return Childs.FilterByPredicate([=](UQuestNode* node)
	{
		return node->CkeckForActivate();
	});
}

void UQuestNode::Prepare(UQuestProcessor* processor, UQuestAsset* quest)
{
	check(processor);
	check(quest);

	Processor = processor;
	OwnerQuest = quest;
	for (auto child : Childs)
	{
		child->Prepare(processor, quest);
	}
}

void UQuestNode::Activate()
{
	OwnerQuest->ActiveNodes.Add(this);
	Status = EQuestCompleteStatus::Active;

	if (TryComplete())
		return;

	if (Stage.WaitHasKeys.Num() != 0 ||
		Stage.WaitDontHasKeys.Num() != 0 ||
		Stage.FailedIfGiveKeys.Num() != 0 ||
		Stage.FailedIfRemoveKeys.Num() != 0)
	{
		Processor->StoryKeyManager->OnKeyRemoveBP.AddDynamic(this, &UQuestNode::OnChangeStoryKey);
		Processor->StoryKeyManager->OnKeyAddBP.AddDynamic(this, &UQuestNode::OnChangeStoryKey);
	}
}

void UQuestNode::OnChangeStoryKey(const FName& key)
{
	if (Stage.WaitHasKeys.Contains(key) ||
		Stage.WaitDontHasKeys.Contains(key) ||
		Stage.FailedIfGiveKeys.Contains(key) ||
		Stage.FailedIfRemoveKeys.Contains(key))
	{
		TryComplete();
	}
}

bool UQuestNode::TryComplete() //todo:: use WaitAllOwner
{
	if (CkeckForFailed())
	{
		Status = EQuestCompleteStatus::Failed;
	}
	else
	{
		if (!CkeckForComplete())
			return false;

		Status = EQuestCompleteStatus::Completed;

		for (auto key : Stage.GiveKeys)
			Processor->StoryKeyManager->AddKey(key);

		for (auto key : Stage.RemoveKeys)
			Processor->StoryKeyManager->RemoveKey(key);

		for (auto& Event : Stage.Action)
			Event.Invoke(this);
	}

	CompleteNode();
	return true;
}

void UQuestNode::CompleteNode()
{
	OwnerQuest->ActiveNodes.Remove(this);
	OwnerQuest->ArchiveNodes.Add(this);

	Processor->CompleteStage(this);

	Processor->StoryKeyManager->OnKeyRemoveBP.RemoveDynamic(this, &UQuestNode::OnChangeStoryKey);
	Processor->StoryKeyManager->OnKeyAddBP.RemoveDynamic(this, &UQuestNode::OnChangeStoryKey);
}

bool UQuestNode::CkeckForActivate()
{
	for (auto key : Stage.CheckHasKeys)
	{
		if (Processor->StoryKeyManager->DontHasKey(key))
			return false;
	}

	for (auto key : Stage.CheckDontHasKeys)
	{
		if (Processor->StoryKeyManager->HasKey(key))
			return false;
	}

	for (auto& Conditions : Stage.Predicate)
	{
		if (!Conditions.InvokeCheck(this))
			return false;
	}

	return true;
}

bool UQuestNode::CkeckForComplete()
{
	for (auto key : Stage.WaitHasKeys)
	{
		if (Processor->StoryKeyManager->DontHasKey(key))
			return false;
	}

	for (auto key : Stage.WaitDontHasKeys)
	{
		if (Processor->StoryKeyManager->HasKey(key))
			return false;
	}

	for (auto& Conditions : Stage.WaitPredicate)
	{
		if (!Conditions.InvokeCheck(this))
			return false;
	}

	return true;
}

bool UQuestNode::CkeckForFailed()
{
	for (auto key : Stage.FailedIfGiveKeys)
	{
		if (Processor->StoryKeyManager->HasKey(key))
			return true;
	}

	for (auto key : Stage.FailedIfRemoveKeys)
	{
		if (Processor->StoryKeyManager->DontHasKey(key))
			return true;
	}

	for (auto& Conditions : Stage.FailedPredicate)
	{
		if (Conditions.InvokeCheck(this))
			return true;
	}

	return false;
}