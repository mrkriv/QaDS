#include "DialogSystemEditor.h"
#include "GraphEditor.h"
#include "SInlineEditableTextBlock.h"
#include "AssetThumbnail.h"
#include "AssetRegistryModule.h"
#include "DialogSGraphNodes.h"
#include "QaDSSettings.h"
#include "SNumericEntryBox.h"
#include "GraphEditor.h"
#include "BrushSet.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"

//PhraseNode.......................................................................................................
void SGraphNode_Phrase::Construct(const FArguments& InArgs, UDialogPhraseEdGraphNode* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

FName SGraphNode_Phrase::GetIcon() const
{
	auto phraseNode = CastChecked<UDialogPhraseEdGraphNode>(GraphNode);

	if (phraseNode->Data.Source == EDialogPhraseSource::Player)
	{
		return "DialogSystem.Player";
	}
	else
	{
		return "DialogSystem.NPC";
	}
}

void SGraphNode_Phrase::CreateNodeWidget()
{
	auto phraseNode = CastChecked<UDialogPhraseEdGraphNode>(GraphNode);

	SGraphNode_QaDSNodeBase::CreateNodeWidget();
	
	for (auto key : phraseNode->Data.CheckHasKeys)
		AddTextToContent(ConditionsBox, TEXT("HAS KEY ") + key.ToString(), FColor(170, 255, 0));

	for (auto key : phraseNode->Data.CheckDontHasKeys)
		AddTextToContent(ConditionsBox, TEXT("HAS NOT KEY ") + key.ToString(), FColor(255, 150, 0));

	for (auto key : phraseNode->Data.Predicate)
		AddTextToContent(ConditionsBox, TEXT("IF ") + key.ToString(), FColor(255, 255, 0));
	
	for (auto key : phraseNode->Data.Action)
		AddTextToContent(EventsBox, TEXT("E ") + key.ToString(), FColor(0, 170, 255));

	for (auto key : phraseNode->Data.GiveKeys)
		AddTextToContent(EventsBox, TEXT("+ ") + key.ToString(), FColor(0, 255, 0));

	for (auto key : phraseNode->Data.RemoveKeys)
		AddTextToContent(EventsBox, TEXT("- ") + key.ToString(), FColor(255, 0, 0));

	ConditionsBox->SetVisibility(ConditionsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
	EventsBox->SetVisibility(EventsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
}

FReply SGraphNode_Phrase::OnClickedIcon()
{
	auto phraseNode = CastChecked<UDialogPhraseEdGraphNode>(GraphNode);

	if (phraseNode->Data.Source == EDialogPhraseSource::Player)
	{
		phraseNode->Data.Source = EDialogPhraseSource::NPC;
	}
	else
	{
		phraseNode->Data.Source = EDialogPhraseSource::Player;
	}

	NodeIcon->SetImage(FBrushSet::Get().GetBrush(GetIcon()));

	return FReply::Handled();
}