#pragma once

#include "EdGraphUtilities.h"

class FQuestEditorNodeFactory : public FGraphPanelNodeFactory
{
public:
	FQuestEditorNodeFactory();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
