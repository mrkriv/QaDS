#include "QuestNode.h"
#include "QuestAsset.h"
#include "QuestProcessor.h"
#include "StoryInformationManager.h"

TArray<UQuestRuntimeNode*> UQuestRuntimeNode::GetNextStage()
{
	if (childCahe.Num() == 0)
	{
		for (auto child : Childs)
		{
			childCahe.Add(OwnerQuest->LoadNode(child));
		}
	}

	return childCahe.FilterByPredicate([=](UQuestRuntimeNode* node)
	{
		return node->CkeckForActivate();
	});
}

void UQuestRuntimeNode::OnChangeStoryKey(const FName& key)
{
	if (Stage.WaitHasKeys.Contains(key)		 ||
		Stage.WaitDontHasKeys.Contains(key)  ||
		Stage.FailedIfGiveKeys.Contains(key) ||
		Stage.FailedIfRemoveKeys.Contains(key))
	{
		TryComplete();
	}
}

void UQuestRuntimeNode::OnTrigger(const FStoryTrigger& Trigger)
{
	for (auto& cond : Stage.WaitTriggers)
	{
		if (MatchTringger(cond, Trigger))
			break;
	}

	for (auto& cond : Stage.FailedTriggers)
	{
		if (MatchTringger(cond, Trigger))
			break;
	}
}

bool UQuestRuntimeNode::TryComplete()
{
	if (CkeckForFailed())
	{
		SetStatus(EQuestCompleteStatus::Failed);
	}
	else
	{
		if (!CkeckForComplete())
			return false;

		SetStatus(EQuestCompleteStatus::Completed);
	}
	return true;
}

void UQuestRuntimeNode::SetStatus(EQuestCompleteStatus NewStatus)
{
	if (NewStatus == Status)
		return;

	Status = NewStatus;

	switch (Status)
	{
	case EQuestCompleteStatus::None:
		break;
	case EQuestCompleteStatus::Active:
		Activate();
		break;
	case EQuestCompleteStatus::Completed:
		Complete();
		Deactivate();
		break;
	case EQuestCompleteStatus::Failed:
		Failed();
		Deactivate();
		break;
	case EQuestCompleteStatus::Skiped:
		Deactivate();
		break;
	}
}

void UQuestRuntimeNode::Activate()
{
	OwnerQuest->ActiveNodes.Add(this);

	if (TryComplete())
		return;

	if (Stage.WaitHasKeys.Num()			> 0 ||
		Stage.WaitDontHasKeys.Num()		> 0 ||
		Stage.FailedIfGiveKeys.Num()	> 0 ||
		Stage.FailedIfRemoveKeys.Num()	> 0)
	{
		Processor->StoryKeyManager->OnKeyRemoveBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
		Processor->StoryKeyManager->OnKeyAddBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	}

	if (Stage.FailedTriggers.Num() > 0 || Stage.WaitTriggers.Num() > 0)
	{
		Processor->StoryTriggerManager->OnTriggerInvoke.AddDynamic(this, &UQuestRuntimeNode::OnTrigger);
	}
}

void UQuestRuntimeNode::Failed()
{
	if (Stage.bFailedQuest || OwnerQuest->ActiveNodes.Num() == 1)
	{
		Processor->EndQuest(OwnerQuest, EQuestCompleteStatus::Failed);
	}
}

void UQuestRuntimeNode::Complete()
{
	if (Stage.ChangeOderActiveStagesState != EQuestCompleteStatus::None)
	{
		auto nodes = OwnerQuest->ActiveNodes;
		for (auto stage : nodes)
		{
			if (stage == this)
				continue;

			stage->SetStatus(Stage.ChangeOderActiveStagesState);
		}
	}

	if (Stage.ChangeQuestState != EQuestCompleteStatus::None)
	{
		Processor->EndQuest(OwnerQuest, Stage.ChangeQuestState);
	}

	for (auto key : Stage.GiveKeys)
		Processor->StoryKeyManager->AddKey(key);

	for (auto key : Stage.RemoveKeys)
		Processor->StoryKeyManager->RemoveKey(key);

	for (auto& Event : Stage.Action)
		Event.Invoke(this);
}

void UQuestRuntimeNode::Deactivate()
{
	OwnerQuest->ActiveNodes.Remove(this);
	OwnerQuest->ArchiveNodes.Add(this);

	Processor->CompleteStage(this);

	Processor->StoryKeyManager->OnKeyRemoveBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	Processor->StoryKeyManager->OnKeyAddBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
	Processor->StoryTriggerManager->OnTriggerInvoke.RemoveDynamic(this, &UQuestRuntimeNode::OnTrigger);
}

bool UQuestRuntimeNode::MatchTringger(FStoryTriggerCondition& condition, const FStoryTrigger& trigger)
{
	if (condition.TriggerName != trigger.TriggerName)
		return false;

	for (auto kpv : trigger.Params)
	{
		if (!condition.ParamsMasks.Contains(kpv.Key))
			return false;

		auto filter = condition.ParamsMasks[kpv.Key];

		if (!MatchTringgerParam(kpv.Value, filter))
			return false;
	}

	condition.TotalCount -= trigger.Count;

	if (condition.TotalCount <= 0)
		TryComplete();

	return true;
}

bool UQuestRuntimeNode::MatchTringgerParam(const FString& value, const FString& filter)
{
	if (filter == "*")
		return true;

	if (filter == value)
		return true;

	return false;
}

bool UQuestRuntimeNode::CkeckForActivate()
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

bool UQuestRuntimeNode::CkeckForComplete()
{
	for (auto& cond : Stage.WaitTriggers)
	{
		if (cond.TotalCount != 0)
			return false;
	}

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

bool UQuestRuntimeNode::CkeckForFailed()
{
	for (auto& cond : Stage.FailedTriggers)
	{
		if (cond.TotalCount == 0)
			return true;
	}

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

FString FStoryTriggerCondition::ToString() const
{
	auto result = TriggerName.ToString() + "[";

	TArray<FString> params;
	ParamsMasks.GenerateValueArray(params);

	for (int i = 0; i < params.Num(); i++)
	{
		if (i != 0)
			result += ", ";

		result += params[i];
	}

	result += "]";

	if(TotalCount > 1)
		result += " x" + FString::FromInt(TotalCount);

	return result;
}