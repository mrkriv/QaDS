#include "DialogSystemEditor.h"
#include "GraphPanelNodeFactory_Dialog.h"
#include "DialogSGraphNodes.h"
#include "DialogEditorNodes.h"

FGraphPanelNodeFactory_Dialog::FGraphPanelNodeFactory_Dialog()
{
}

TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_Dialog::CreateNode(UEdGraphNode* Node) const
{
	TSharedPtr<SGraphNode_DialogNodeBase> SNode;

	if (Node->IsA(UPhraseNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_Phrase, Cast<UPhraseNode>(Node));
	}
	else if (Node->IsA(UWaitNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_Wait, Cast<UWaitNode>(Node));
	}
	else if (Node->IsA(URootNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_Root, Cast<URootNode>(Node));
	}

	if (Node != NULL && Node->IsA(UDialogNodeEditorBase::StaticClass()))
		Cast<UDialogNodeEditorBase>(Node)->PropertyObserver = SNode;

	return SNode;
}