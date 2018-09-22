#pragma once

#include "QaDSSlateNode.h"
#include "DialogEditorNodes.h"

//RootNode.............................................................................................................
class SGraphNode_Root : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.Root"; }
};

//PhraseNode..................................................................................................................
class SGraphNode_Phrase : public SGraphNode_QaDSNodeBase
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_Phrase) { }
	SLATE_END_ARGS()

	virtual void Construct(const FArguments& InArgs, UDialogPhraseEdGraphNode* InNode);
	virtual void CreateNodeWidget() override;
	virtual FName GetIcon() const override;
	virtual FReply OnClickedIcon() override;
};

//SubGraph.............................................................................................................
class SGraphNode_SubGraph : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.SubGraph"; }
};

//ElseIf.............................................................................................................
class SGraphNode_ElseIf : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.ElseIf"; }
};