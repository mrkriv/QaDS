#pragma once

#include "QaDSSlateNode.h"

//StageNode..................................................................................................................
class SGraphNode_QuestNode : public SGraphNode_QaDSNodeBase
{
public:
	virtual void CreateNodeWidget() override;
	virtual FName GetIcon() const override;
};

//QuestEndNode..................................................................................................................
class SGraphNode_QuestRootNode : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override;
};