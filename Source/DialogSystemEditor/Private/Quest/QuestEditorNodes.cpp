#include "DialogSystemEditor.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "QuestEditorNodes.h"
#include "QuestAsset.h"
#include "XmlFile.h"

//UQuestStageEdGraphNode...........................................................................................
FString UQuestStageEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestStageEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

//UQuestEndEdGraphNode...........................................................................................
FString UQuestEndEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestEndEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

//UQuestEndEdGraphNode...........................................................................................
FString UQuestRootEdGraphNode::SaveToXml(int tabLevel) const
{
	FString xml;
	return xml;
}

void UQuestRootEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}
