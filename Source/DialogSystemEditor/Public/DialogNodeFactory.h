#pragma once

#include "EdGraphUtilities.h"

class  FDialogNodeFactory : public FGraphPanelNodeFactory
{
public:
	FDialogNodeFactory();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
