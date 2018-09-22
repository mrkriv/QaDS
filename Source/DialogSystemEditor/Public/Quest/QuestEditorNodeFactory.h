#pragma once

#include "EdGraphUtilities.h"

class FQuestEditorNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
