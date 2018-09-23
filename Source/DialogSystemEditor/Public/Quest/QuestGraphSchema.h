#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "DialogGraphSchema.h"
#include "QuestGraphSchema.generated.h"

UCLASS()
class UQuestGraphSchema : public UDialogGraphSchema
{
	GENERATED_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
};
