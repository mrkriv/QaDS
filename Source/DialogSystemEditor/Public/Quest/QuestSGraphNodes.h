#pragma once

#include "QaDSSlateNode.h"

//StageNode..................................................................................................................
class SGraphNode_QuestNode : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.Stage"; }
	//virtual FName GetIcon() const override { return "DialogSystem.HasOne"; }
	//virtual FName GetIcon() const override { return "DialogSystem.HasAll"; }
};

//QuestEndNode..................................................................................................................
class SGraphNode_QuestEndNode : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.Feiled"; }
};

//QuestEndNode..................................................................................................................
class SGraphNode_QuestRootNode : public SGraphNode_QaDSNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.Book"; }
};