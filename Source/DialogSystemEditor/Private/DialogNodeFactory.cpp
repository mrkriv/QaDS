// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "DialogNodeFactory.h"
#include "DialogSGraphNodes.h"
#include "DialogEditorNodes.h"

FDialogNodeFactory::FDialogNodeFactory()
{
}

TSharedPtr<class SGraphNode> FDialogNodeFactory::CreateNode(UEdGraphNode* Node) const
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