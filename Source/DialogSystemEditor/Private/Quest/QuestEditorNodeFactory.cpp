#include "DialogSystemEditor.h"
#include "QuestEditorNodeFactory.h"
#include "QuestSGraphNodes.h"
#include "QuestEditorNodes.h"

FQuestEditorNodeFactory::FQuestEditorNodeFactory()
{
}

TSharedPtr<class SGraphNode> FQuestEditorNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	TSharedPtr<SGraphNode> SNode;

	/*if (Node->IsA(UQuestPhraseEdGraphNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_Phrase, Cast<UQuestPhraseEdGraphNode>(Node));
	}*/
	
	return SNode;
}