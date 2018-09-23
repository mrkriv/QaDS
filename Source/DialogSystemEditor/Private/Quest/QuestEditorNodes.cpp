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

FXmlWriteNode UQuestStageEdGraphNode::SaveToXml() const
{
	return Super::SaveToXml();
}

void UQuestStageEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

//UQuestEndEdGraphNode...........................................................................................
void UQuestEndEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
}

FXmlWriteNode UQuestEndEdGraphNode::SaveToXml() const
{
	return Super::SaveToXml();
}

void UQuestEndEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

//UQuestRootEdGraphNode...........................................................................................
void UQuestRootEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FXmlWriteNode UQuestRootEdGraphNode::SaveToXml() const
{
	return Super::SaveToXml();
}

void UQuestRootEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}
