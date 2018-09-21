#include "DialogSystemEditor.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "QuestEditorNodes.h"
#include "QuestAsset.h"
#include "XmlFile.h"

//UQuestEdGraphNode...........................................................................................
TArray<UQuestEdGraphNode*> UQuestEdGraphNode::GetChildNodes() const
{
	TArray<UQuestEdGraphNode*> ChildNodes;

	for (auto Pin : Pins)
	{
		if (Pin->Direction != EGPD_Output)
			continue;

		for (int i = 0; i < Pin->LinkedTo.Num(); i++)
		{
			if (Pin->LinkedTo[i])
				ChildNodes.Add((UQuestEdGraphNode*)Pin->LinkedTo[i]->GetOwningNode());
		}
	}

	return ChildNodes;
}

void UQuestEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyObserver.IsValid())
	{
		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		PropertyObserver->OnPropertyChanged(this, PropertyName);
	}

	Super::PostEditChangeProperty(e);
}

int UQuestEdGraphNode::GetOrder() const
{
	auto inputPin = Pins.FindByPredicate([](const UEdGraphPin* pin) {return pin->Direction == EGPD_Input; });

	if(inputPin == NULL || (*inputPin)->LinkedTo.Num() == 0)
		return 0;
	
	auto bigOwner = (UQuestEdGraphNode*)(*inputPin)->LinkedTo[0]->GetOwningNode();
	for(auto ownerPin : (*inputPin)->LinkedTo)
	{
		auto owner = (UQuestEdGraphNode*)ownerPin->GetOwningNode();

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

FString UQuestEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQuestEdGraphNode*>& nodeById)
{
}
