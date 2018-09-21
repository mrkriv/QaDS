#pragma once

#include "EdGraphUtilities.h"

class  FDialogEditorNodeFactory : public FGraphPanelNodeFactory
{
public:
	FDialogEditorNodeFactory();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
