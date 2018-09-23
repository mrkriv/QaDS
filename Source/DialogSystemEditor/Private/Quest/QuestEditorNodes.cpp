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

FString UQuestStageEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestStageEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

//UQuestEndEdGraphNode...........................................................................................
void UQuestEndEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
}

FString UQuestEndEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestEndEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

//UQuestRootEdGraphNode...........................................................................................
void UQuestRootEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FString UQuestRootEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestRootEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}
