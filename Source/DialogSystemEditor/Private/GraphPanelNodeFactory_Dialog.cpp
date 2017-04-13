#include "DialogSystemEditor.h"
#include "GraphPanelNodeFactory_Dialog.h"
#include "DialogSGraphNodes.h"
#include "DialogEditorNodes.h"

FGraphPanelNodeFactory_Dialog::FGraphPanelNodeFactory_Dialog()
{
}

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Dialog::CreateNode(UEdGraphNode* Node) const
{
	if (URootNode* RootNode = Cast<URootNode>(Node))
	{
		TSharedPtr<SGraphNode_Root> SNode = SNew(SGraphNode_Root, RootNode);
		RootNode->PropertyObserver = SNode;
		return SNode;
	}
	else if (UPhraseNode* PhraseNode = Cast<UPhraseNode>(Node))
	{
		TSharedPtr<SGraphNode_Phrase> SNode = SNew(SGraphNode_Phrase, PhraseNode);
		PhraseNode->PropertyObserver = SNode;
		return SNode;
	}
	return NULL;
}
