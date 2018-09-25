#include "DialogSystemRuntime.h"
#include "QuestNode.h"
#include "QuestAsset.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"

void UQuestNode::InvokePostScript(UQuestProcessor* processor)
{
	check(processor);
	//todo
}

void UQuestNode::SetOwnerQuest(UQuestAsset* quest)
{
	OwnerQuest = quest;
	for (auto child : Childs)
	{
		child->SetOwnerQuest(quest);
	}
}

bool UQuestNode::TryComplete(UQuestProcessor* processor)
{
	check(processor);

	if (!CkeckForComplete(processor))
		return false;

	processor->ActivateStage(this);
	//todo:: use WaitAllOwner
	
	return true;
}

bool UQuestNode::CkeckForActivate(UQuestProcessor* processor)
{
	check(processor);
	//todo
	return true;
}

bool UQuestNode::CkeckForComplete(UQuestProcessor* processor)
{
	check(processor);

	if (Stage.WaitDontHasKeys.Num() + Stage.WaitHasKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager();

		for (auto key : Stage.WaitHasKeys)
		{
			if (skm->DontHasKey(key))
				return false;
		}

		for (auto key : Stage.WaitDontHasKeys)
		{
			if (skm->HasKey(key))
				return false;
		}
	}

	//todo:: WaitPredicate

	return true;
}

void UQuestNode::Assign(UQuestProcessor* processor)
{
	check(processor);

	if (Stage.WaitDontHasKeys.Num() + Stage.WaitHasKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager();
		auto onMyKeyChange = [this](FName key)
		{
			if (Stage.WaitHasKeys.Contains(key) || Stage.WaitDontHasKeys.Contains(key))
			{
				TryComplete(UQuestProcessor::GetQuestProcessor());
			}
		};

		skm->OnKeyRemove.AddLambda(onMyKeyChange);
		skm->OnKeyAdd.AddLambda(onMyKeyChange);
	}
}

TArray<UQuestNode*> UQuestNode::GetNextStage(UQuestProcessor* processor)
{
	check(processor);

	return Childs.FilterByPredicate([=](UQuestNode* node)
	{
		return node->CkeckForActivate(processor);
	});
}
