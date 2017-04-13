#pragma once
#include "EdGraphUtilities.h"

class  FGraphPanelNodeFactory_Dialog : public FGraphPanelNodeFactory
{
public:
	FGraphPanelNodeFactory_Dialog();

private:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;
};
