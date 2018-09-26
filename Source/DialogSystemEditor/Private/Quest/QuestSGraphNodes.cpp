#include "DialogSystemEditor.h"
#include "GraphEditor.h"
#include "SInlineEditableTextBlock.h"
#include "AssetThumbnail.h"
#include "AssetRegistryModule.h"
#include "SNumericEntryBox.h"
#include "GraphEditor.h"
#include "BrushSet.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"
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
		return stage.WaitAllOwner ? "DialogSystem.HasAll" : "DialogSystem.HasOne";
	}

	return "DialogSystem.Stage";
}

void SGraphNode_QuestNode::CreateNodeWidget()
{
	SGraphNode_QaDSNodeBase::CreateNodeWidget();

	auto& stage = CastChecked<UQuestStageEdGraphNode>(GraphNode)->Stage;

	for (auto key : stage.CheckHasKeys)
		AddTextToContent(ConditionsBox, TEXT("HAS KEY ") + key.ToString(), FColor(170, 255, 0));

	for (auto key : stage.CheckDontHasKeys)
		AddTextToContent(ConditionsBox, TEXT("HAS NOT KEY ") + key.ToString(), FColor(255, 150, 0));

	for (auto key : stage.Predicate)
		AddTextToContent(ConditionsBox, TEXT("IF ") + key.ToString(), FColor(255, 255, 0));


	for (auto key : stage.WaitHasKeys)
		AddTextToContent(BodyBox, TEXT("Wait Give Key ") + key.ToString(), FColor(170, 255, 0));

	for (auto key : stage.WaitDontHasKeys)
		AddTextToContent(BodyBox, TEXT("Wait Remove Key ") + key.ToString(), FColor(255, 150, 0));

	for (auto key : stage.WaitPredicate)
		AddTextToContent(BodyBox, TEXT("Wait ") + key.ToString(), FColor(255, 255, 0));


	AddTextToContent(BodyBox, TEXT(""), FColor(0, 0, 0));
	for (auto key : stage.FailedIfGiveKeys)
		AddTextToContent(BodyBox, TEXT("Failed if give key ") + key.ToString(), FColor(255, 32, 32));

	for (auto key : stage.FailedIfRemoveKeys)
		AddTextToContent(BodyBox, TEXT("Failed if remove key ") + key.ToString(), FColor(255, 32, 32));

	for (auto key : stage.FailedPredicate)
		AddTextToContent(BodyBox, TEXT("Failed if ") + key.ToString(), FColor(255, 32, 32));


	for (auto key : stage.Action)
		AddTextToContent(EventsBox, TEXT("E ") + key.ToString(), FColor(0, 170, 255));

	for (auto key : stage.GiveKeys)
		AddTextToContent(EventsBox, TEXT("+ ") + key.ToString(), FColor(32, 255, 32));

	for (auto key : stage.RemoveKeys)
		AddTextToContent(EventsBox, TEXT("- ") + key.ToString(), FColor(255, 32, 32));

	ConditionsBox->SetVisibility(ConditionsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
	EventsBox->SetVisibility(EventsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
}