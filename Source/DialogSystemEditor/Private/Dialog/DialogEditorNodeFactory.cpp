#include "DialogSystemEditor.h"
#include "DialogEditorNodeFactory.h"
#include "DialogSGraphNodes.h"
#include "DialogEditorNodes.h"

TSharedPtr<class SGraphNode> FDialogEditorNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	TSharedPtr<SGraphNode_QaDSNodeBase> SNode;

	if (Node->IsA(UDialogPhraseEdGraphNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_Phrase, Cast<UDialogPhraseEdGraphNode>(Node));
	}
	else if (Node->IsA(UDialogRootEdGraphNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_Root, Cast<UDialogRootEdGraphNode>(Node));
	}
	else if (Node->IsA(UDialogSubGraphEdGraphNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_SubGraph, Cast<UDialogSubGraphEdGraphNode>(Node));
	}
	else if (Node->IsA(UDialogElseIfEdGraphNode::StaticClass()))
	{
		SNode = SNew(SGraphNode_ElseIf, Cast<UDialogElseIfEdGraphNode>(Node));
	}

	if (Node != NULL && Node->IsA(UDialogEdGraphNode::StaticClass()))
		Cast<UDialogEdGraphNode>(Node)->PropertyObserver = SNode;

	return SNode;
}