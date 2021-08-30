#include "SNumericEntryBox.h"
#include "QuestSGraphNodes.h"
#include "QuestEditorNodes.h"

//SGraphNode_QuestRootNode..................................................................................................................
FName SGraphNode_QuestRootNode::GetIcon() const
{
	return "DialogSystem.QuestRoot";
}

//SGraphNode_QuestNode..................................................................................................................
FName SGraphNode_QuestNode::GetIcon() const
{
	auto node = CastChecked<UQuestStageEdGraphNode>(GraphNode);
	auto& stage = node->Stage;

	if (stage.ChangeQuestState == EQuestCompleteStatus::Completed)
	{
		return "DialogSystem.Completed";
	}

	if (stage.ChangeQuestState == EQuestCompleteStatus::Failed)
	{
		return "DialogSystem.Feiled";
	}

	if (node->GetChildNodes().Num() > 1)
	{
		if(node->Stage.ChangeOderActiveStagesState == EQuestCompleteStatus::Skiped)
			return "DialogSystem.HasOne";

		return "DialogSystem.HasAll";
	}

	return "DialogSystem.Stage";
}

void SGraphNode_QuestNode::CreateNodeWidget()
{
	SGraphNode_QaDSNodeBase::CreateNodeWidget();

	auto& stage = CastChecked<UQuestStageEdGraphNode>(GraphNode)->Stage;

	for (auto key : stage.CheckHasKeys)
		AddTextToContent(ConditionsBox, TEXT(""), key.ToString(), FColor(170, 255, 0));

	for (auto key : stage.CheckDontHasKeys)
		AddTextToContent(ConditionsBox, TEXT("!"), key.ToString(), FColor(255, 150, 0));

	for (auto key : stage.Predicate)
		AddTextToContent(ConditionsBox, TEXT("IF"), key.ToString(), FColor(255, 255, 0));


	for (auto key : stage.WaitHasKeys)
		AddTextToContent(BodyBox, TEXT("Wait give key"), key.ToString(), FColor(170, 255, 0));

	for (auto key : stage.WaitDontHasKeys)
		AddTextToContent(BodyBox, TEXT("Wait remove key"), key.ToString(), FColor(255, 150, 0));

	for (auto key : stage.WaitPredicate)
		AddTextToContent(BodyBox, TEXT("Wait"), key.ToString(), FColor(255, 255, 0));

	for (auto key : stage.WaitTriggers)
		AddTextToContent(BodyBox, TEXT("Wait on"), key.ToString(), FColor(255, 255, 0));

	
	for (auto key : stage.FailedIfGiveKeys)
		AddTextToContent(BodyBox, TEXT("Failed if give key"), key.ToString(), FColor(255, 32, 32));

	for (auto key : stage.FailedIfRemoveKeys)
		AddTextToContent(BodyBox, TEXT("Failed if remove key"), key.ToString(), FColor(255, 32, 32));

	for (auto key : stage.FailedPredicate)
		AddTextToContent(BodyBox, TEXT("Failed if"), key.ToString(), FColor(255, 32, 32));

	for (auto key : stage.FailedTriggers)
		AddTextToContent(BodyBox, TEXT("Failed on"), key.ToString(), FColor(255, 32, 32));


	if (stage.ChangeQuestState != EQuestCompleteStatus::None)
		AddTextToContent(EventsBox, TEXT("Change quest state"), "", FColor(0, 170, 255));

	if (stage.ChangeOderActiveStagesState != EQuestCompleteStatus::None)
		AddTextToContent(EventsBox, TEXT("Change oder stages state"), "", FColor(0, 170, 255));

	for (auto key : stage.Action)
		AddTextToContent(EventsBox, TEXT(""), key.ToString(), FColor(0, 170, 255));

	for (auto key : stage.GiveKeys)
		AddTextToContent(EventsBox, TEXT("+"), key.ToString(), FColor(32, 255, 32));

	for (auto key : stage.RemoveKeys)
		AddTextToContent(EventsBox, TEXT("-"), key.ToString(), FColor(255, 32, 32));

	ConditionsBox->SetVisibility(ConditionsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
	EventsBox->SetVisibility(EventsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
}