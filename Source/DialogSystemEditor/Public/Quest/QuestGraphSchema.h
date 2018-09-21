#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "DialogGraphSchema.h"
#include "QuestGraphSchema.generated.h"

UCLASS()
class UQuestGraphSchema : public UDialogGraphSchema
{
	GENERATED_BODY()

public:
	// Begin EdGraphSchema interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	// End EdGraphSchema interface
};
