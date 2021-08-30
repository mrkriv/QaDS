#include "QuestEditorNodes.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "QuestGraphSchema.h"

#include "DialogEditorNodes.h"
#include "QaDSGraphSchema.h"
#include "Developer/ToolMenus/Public/ToolMenuSection.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "FQuestSystemModule"

void UQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	auto FromPin = ContextMenuBuilder.FromPin;
	auto Graph = ContextMenuBuilder.CurrentGraph;
	auto OwnerOfTemp = ContextMenuBuilder.OwnerOfTemporaries;

	bool rootFound = false;
	for (auto node : Graph->Nodes)
	{
		if (Cast<UQuestRootEdGraphNode>(node))
		{
			rootFound = true;
			break;
		}
	}

	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;

	if (!rootFound)
		QaDSSchemaUtils::AddAction<UQuestRootEdGraphNode>(TEXT("Create Root Node"), TEXT(""), Actions, OwnerOfTemp);

	QaDSSchemaUtils::AddAction<UQuestStageEdGraphNode>(TEXT("Add Stage"), TEXT(""), Actions, OwnerOfTemp);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
		ContextMenuBuilder.AddAction(Action);
}

void UQuestGraphSchema::GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection Section = Menu->AddSection("Edit");
	
	Section.AddMenuEntry(FGenericCommands::Get().Delete);
	Section.AddMenuEntry(FGenericCommands::Get().Cut);
	Section.AddMenuEntry(FGenericCommands::Get().Copy);
	Section.AddMenuEntry(FGenericCommands::Get().Paste);
	Section.AddMenuEntry(FGenericCommands::Get().SelectAll);
	
	if (!Cast<UDialogRootEdGraphNode>(Context->Node))
		Section.AddMenuEntry(FGenericCommands::Get().Duplicate);

	if (Context->Pin)
		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
	else
		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);

	Super::GetContextMenuActions(Menu, Context);
}

#undef LOCTEXT_NAMESPACE