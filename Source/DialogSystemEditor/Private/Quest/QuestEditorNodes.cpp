#include "QuestEditorNodes.h"
#include "QuestAsset.h"

//UQuestStageEdGraphNode...........................................................................................
void UQuestStageEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
	CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FText UQuestStageEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Stage.Caption.IsEmpty())
		return FText::FromString("Quest Stage");

	return Stage.Caption;
}

FXmlWriteNode UQuestStageEdGraphNode::SaveToXml() const
{
	auto node = Super::SaveToXml();

	node.Append("caption", Stage.Caption);
	node.Append("description", Stage.Description);
	node.Append("aditional_data", Stage.AditionalData);
	node.Append("generate_events", Stage.bGenerateEvents);
	node.Append("check_has_keys", Stage.CheckHasKeys);
	node.Append("check_dont_has_keys", Stage.CheckDontHasKeys);
	node.Append("predicate", Stage.Predicate);
	node.Append("wait_has_keys", Stage.WaitHasKeys);
	node.Append("wait_dont_has_keys", Stage.WaitDontHasKeys);
	node.Append("wait_triggers", Stage.WaitTriggers);
	node.Append("wait_predicate", Stage.WaitPredicate);
	node.Append("failed_if_give_keys", Stage.FailedIfGiveKeys);
	node.Append("failed_if_remove_keys", Stage.FailedIfRemoveKeys);
	node.Append("failed_triggers", Stage.FailedTriggers);
	node.Append("failed_predicate", Stage.FailedPredicate);
	node.Append("failed_ouest", Stage.bFailedQuest);
	node.Append("give_keys", Stage.GiveKeys);
	node.Append("remove_keys", Stage.RemoveKeys);
	node.Append("action", Stage.Action);
	node.Append("change_quest_state", (int)Stage.ChangeQuestState);
	node.Append("change_oder_active_stages_state", (int)Stage.ChangeOderActiveStagesState);

	return node;
}

void UQuestStageEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	Super::LoadInXml(reader, nodeById);

	reader->TryGet("caption", Stage.Caption);
	reader->TryGet("description", Stage.Description);
	reader->TryGet("aditional_data", Stage.AditionalData);
	reader->TryGet("generate_events", Stage.bGenerateEvents);
	reader->TryGet("check_has_keys", Stage.CheckHasKeys);
	reader->TryGet("check_dont_has_keys", Stage.CheckDontHasKeys);
	reader->TryGet("predicate", Stage.Predicate);
	reader->TryGet("wait_has_keys", Stage.WaitHasKeys);
	reader->TryGet("wait_dont_has_keys", Stage.WaitDontHasKeys);
	reader->TryGet("wait_triggers", Stage.WaitTriggers);
	reader->TryGet("wait_predicate", Stage.WaitPredicate);
	reader->TryGet("failed_if_give_keys", Stage.FailedIfGiveKeys);
	reader->TryGet("failed_if_remove_keys", Stage.FailedIfRemoveKeys);
	reader->TryGet("failed_triggers", Stage.FailedTriggers);
	reader->TryGet("failed_predicate", Stage.FailedPredicate);
	reader->TryGet("failed_ouest", Stage.bFailedQuest);
	reader->TryGet("give_keys", Stage.GiveKeys);
	reader->TryGet("remove_keys", Stage.RemoveKeys);
	reader->TryGet("action", Stage.Action);
	reader->TryGet("change_quest_state", (int&)Stage.ChangeQuestState);
	reader->TryGet("change_oder_active_stages_state", (int&)Stage.ChangeOderActiveStagesState);
}

//UQuestRootEdGraphNode...........................................................................................
void UQuestRootEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FText UQuestRootEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Quest Start");
}

//FQuestStageEvent..........................................................................................................
void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FStoryTriggerCondition>& tuple)
{
	auto subNode = FXmlWriteNode(tuple.Tag);
	auto& value = tuple.Value;

	subNode.Append("trigger", value.TriggerName);
	subNode.Append("count", value.TotalCount);
	subNode.Append("masks", value.ParamsMasks);

	node.Childrens.Add(subNode);
}

void operator>>(const FXmlReadNode& node, FStoryTriggerCondition& value)
{
	node.TryGet("trigger", value.TriggerName);
	node.TryGet("count", value.TotalCount);
	node.TryGet("masks", value.ParamsMasks);
}

//FQuestStageEvent..........................................................................................................
void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FQuestStageEvent>& tuple)
{
	auto subNode = FXmlWriteNode(tuple.Tag);
	auto& value = tuple.Value;

	subNode.Append("type", (int)value.CallType);
	subNode.Append("event", value.EventName);
	subNode.Append("tag", value.FindTag);
	subNode.Append("command", value.Command);

	if (value.ObjectClass != NULL)
		subNode.Append("class", value.ObjectClass->GetFullName());

	subNode.Append("params", value.Parameters);

	node.Childrens.Add(subNode);
}

void operator>>(const FXmlReadNode& node, FQuestStageEvent& value)
{
	node.TryGet("type", (int&)value.CallType);
	node.TryGet("tag", value.FindTag);
	node.TryGet("command", value.Command);
	node.TryGet("params", value.Parameters);

	value.ObjectClass = FindObject<UClass>(ANY_PACKAGE, *node.Get("class"));
}

//FQuestStageCondition..........................................................................................................
void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FQuestStageCondition>& tuple)
{
	node << FXmlWriteTuple<FQuestStageEvent>(tuple.Tag, tuple.Value);
	node.Childrens.Last().Append("invert", tuple.Value.InvertCondition);
}

void operator>>(const FXmlReadNode& node, FQuestStageCondition& value)
{
	node >> (FQuestStageEvent&)value;
	node.TryGet("invert", value.InvertCondition);
}
