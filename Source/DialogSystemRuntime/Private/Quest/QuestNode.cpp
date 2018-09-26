#include "DialogSystemRuntime.h"
#include "QuestNode.h"
#include "QuestAsset.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"

void UQuestNode::InvokePostScript(UQuestProcessor* processor)
{
	check(processor);

	for (auto key : Stage.GiveKeys)
		processor->StoryKeyManager->AddKey(key);

	for (auto key : Stage.RemoveKeys)
		processor->StoryKeyManager->RemoveKey(key);

	for (auto& Event : Stage.Action)
		Event.Invoke(this);
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

	for (auto key : Stage.CheckHasKeys)
	{
		if (processor->StoryKeyManager->DontHasKey(key))
			return false;
	}

	for (auto key : Stage.CheckDontHasKeys)
	{
		if (processor->StoryKeyManager->HasKey(key))
			return false;
	}

	for (auto& Conditions : Stage.Predicate)
	{
		if (!Conditions.InvokeCheck(this))
			return false;
	}

	return true;
}

bool UQuestNode::CkeckForComplete(UQuestProcessor* processor)
{
	check(processor);

	for (auto key : Stage.WaitHasKeys)
	{
		if (processor->StoryKeyManager->DontHasKey(key))
			return false;
	}

	for (auto key : Stage.WaitDontHasKeys)
	{
		if (processor->StoryKeyManager->HasKey(key))
			return false;
	}

	//todo:: WaitPredicate

	return true;
}

void UQuestNode::Assign(UQuestProcessor* processor)
{
	check(processor);

	if (Stage.WaitDontHasKeys.Num() + Stage.WaitHasKeys.Num() > 0)
	{
		auto onMyKeyChange = [this](FName key)
		{
			if (Stage.WaitHasKeys.Contains(key) || Stage.WaitDontHasKeys.Contains(key))
			{
				TryComplete(UQuestProcessor::GetQuestProcessor());
			}
		};

		processor->StoryKeyManager->OnKeyRemove.AddLambda(onMyKeyChange);
		processor->StoryKeyManager->OnKeyAdd.AddLambda(onMyKeyChange);
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
