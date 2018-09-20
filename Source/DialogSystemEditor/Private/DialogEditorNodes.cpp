#include "DialogSystemEditor.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "DialogEditorNodes.h"
#include "DialogSettings.h"
#include "DialogAsset.h"

//UDdialogEdGraphNode...........................................................................................
TArray<UDdialogEdGraphNode*> UDdialogEdGraphNode::GetChildNodes()
{
	TArray<UDdialogEdGraphNode*> ChildNodes;

	for (auto Pin : Pins)
	{
		if (Pin->Direction != EGPD_Output)
			continue;

		for (int i = 0; i < Pin->LinkedTo.Num(); i++)
		{
			if (Pin->LinkedTo[i])
				ChildNodes.Add((UDdialogEdGraphNode*)Pin->LinkedTo[i]->GetOwningNode());
		}
	}

	return ChildNodes;
}

void UDdialogEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyObserver.IsValid())
	{
		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		PropertyObserver->OnPropertyChanged(this, PropertyName);
	}

	Super::PostEditChangeProperty(e);
}

int UDdialogEdGraphNode::GetOrder() const
{
	auto inputPin = Pins.FindByPredicate([](const UEdGraphPin* pin) {return pin->Direction == EGPD_Input; });

	if(inputPin == NULL || (*inputPin)->LinkedTo.Num() == 0)
		return 0;
	
	auto bigOwner = (UDdialogEdGraphNode*)(*inputPin)->LinkedTo[0]->GetOwningNode();
	for(auto ownerPin : (*inputPin)->LinkedTo)
	{
		auto owner = (UDdialogEdGraphNode*)ownerPin->GetOwningNode();

		if (owner != NULL && owner->GetChildNodes().Num() > bigOwner->GetChildNodes().Num())
		{
			bigOwner = owner;
		}
	}

	auto lessCount = 0;
	for (auto node : bigOwner->GetChildNodes())
	{
		if (node->NodePosX < NodePosX)
			lessCount++;
	}

	return lessCount + 1;
}

//PhraseNode..........................................................................................................
UDialogPhraseEdGraphNode::UDialogPhraseEdGraphNode()
{
	Data.Source = EDialogPhraseSource::NPC;
}

void UDialogPhraseEdGraphNode::AllocateDefaultPins()
{
	InputPin = CreatePin(EGPD_Input, NAME_None, FName("Input"));
	OutputPin = CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FText UDialogPhraseEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Data.Text.IsEmpty())
		return FText::FromString("Phrase");

	return Data.Text;
}

void UDialogPhraseEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UDdialogEdGraphNode::PostEditChangeProperty(e);
}

//UDialogSubGraphEdGraphNode...........................................................................................
void UDialogSubGraphEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	InputPin = CreatePin(EGPD_Input, NAME_None, FName("Input"));
}

FText UDialogSubGraphEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetDialog != NULL)
		return FText::FromName(TargetDialog->GetFName());

	return FText::FromString("Asset is not set!");
}

//UDialogElseIfEdGraphNode...........................................................................................

void UDialogElseIfEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	InputPin = CreatePin(EGPD_Input, NAME_None, FName("Input"));
	OutputPin = CreatePin(EGPD_Output, NAME_None, FName("Default"));
}

FText UDialogElseIfEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Else-if");
}

//UDialogRootEdGraphNode...........................................................................................

UDialogPhraseEdGraphNode_Player::UDialogPhraseEdGraphNode_Player()
{
	Data.Source = EDialogPhraseSource::Player;
}

//UDialogRootEdGraphNode...........................................................................................
void UDialogRootEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	OutputPin = CreatePin(EGPD_Output, NAME_None, FName("Start"));
}

FText UDialogRootEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Start ") + Cast<UDialogAsset>(GetGraph()->GetOuter())->Name.ToString());
}

bool UDialogRootEdGraphNode::CanUserDeleteNode() const
{
	return false;
}