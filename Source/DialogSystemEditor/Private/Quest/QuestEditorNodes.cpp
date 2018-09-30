#include "DialogSystemEditor.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "QuestEditorNodes.h"
#include "QuestAsset.h"
#include "XmlFile.h"

//UQuestStageEdGraphNode...........................................................................................
void UQuestStageEdGraphNode::AllocateDefaultPins()
{
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
	return Super::SaveToXml();
}

void UQuestStageEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
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

FXmlWriteNode UQuestRootEdGraphNode::SaveToXml() const
{
	return Super::SaveToXml();
}

void UQuestRootEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}
