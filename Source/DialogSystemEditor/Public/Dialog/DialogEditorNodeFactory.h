#pragma once

#include "EdGraphUtilities.h"

class  FDialogEditorNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
