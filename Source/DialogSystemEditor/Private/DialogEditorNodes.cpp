// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "DialogEditorNodes.h"
#include "DialogSettings.h"
#include "DialogAsset.h"

const FString FPinDataTypes::PinType_Root = "root";

//UDialogNodeEditorBase...........................................................................................

TArray<UDialogNodeEditorBase*> UDialogNodeEditorBase::GetChildNodes()
{
	TArray<UDialogNodeEditorBase*> ChildNodes;

	for (auto Pin : Pins)
	{
		if (Pin->Direction != EGPD_Output)
			continue;

		for (int i = 0; i < Pin->LinkedTo.Num(); i++)
		{
			if (Pin->LinkedTo[i])
				ChildNodes.Add((UDialogNodeEditorBase*)Pin->LinkedTo[i]->GetOwningNode());
		}
	}

	return ChildNodes;
}

void UDialogNodeEditorBase::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyObserver.IsValid())
	{
		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		PropertyObserver->OnPropertyChanged(this, PropertyName);
	}

	Super::PostEditChangeProperty(e);
}

//PhraseNode..........................................................................................................

UPhraseNode::UPhraseNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Data.Source = EDialogPhraseSource::NPC;
}

void UPhraseNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, NAME_None, FName(""));
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, NAME_None, FName(""));
}

FText UPhraseNode::GetTooltipText() const
{
	return FText::FromString("Dialog Phrase");
}

FText UPhraseNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Data.Text.IsEmpty())
		return FText::FromString("Phrase");

	return Data.Text;
}

void UPhraseNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UDialogNodeEditorBase::PostEditChangeProperty(e);
}

UPhrasePlayerNode::UPhrasePlayerNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Data.Source = EDialogPhraseSource::Player;
}

//URootNode...........................................................................................

URootNode::URootNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URootNode::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, NAME_None, FName("Start"));
}

FText URootNode::GetTooltipText() const
{
	return FText::FromString("Dialog Start Node");
}

FText URootNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Start ") + Cast<UDialogAsset>(GetGraph()->GetOuter())->Name.ToString());
}

bool URootNode::CanUserDeleteNode() const
{
	return false;
}