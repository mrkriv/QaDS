#include "DialogSystemEditor.h"
#include "DialogEditorNodes.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "RectConnectionDrawingPolicy.h"
#include "DialogGraphSchema.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "DialogEditorNodes.h"
#include "QaDSGraphSchema.h"

#define LOCTEXT_NAMESPACE "FDialogSystemModule"

void UDialogGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	auto FromPin = ContextMenuBuilder.FromPin;
	auto Graph = ContextMenuBuilder.CurrentGraph;
	auto OwnerOfTemp = ContextMenuBuilder.OwnerOfTemporaries;

	bool rootFound = false;
	for (auto node : Graph->Nodes)
	{
		if (Cast<UDialogRootEdGraphNode>(node))
		{
			rootFound = true;
			break;
		}
	}

	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;

	if (!rootFound)
		QaDSSchemaUtils::AddAction<UDialogRootEdGraphNode>(TEXT("Create Root Node"), TEXT(""), Actions, OwnerOfTemp);

	QaDSSchemaUtils::AddAction<UDialogPhraseEdGraphNode_Player>(TEXT("Add Player Phrase"), TEXT("Add dialog phrase node"), Actions, OwnerOfTemp);
	QaDSSchemaUtils::AddAction<UDialogPhraseEdGraphNode>(TEXT("Add NPC Phrase"), TEXT("Add dialog phrase node"), Actions, OwnerOfTemp);
	//QaDSSchemaUtils::AddAction<UDialogElseIfEdGraphNode>(TEXT("Add ElseIf"), TEXT("Add else-if node"), Actions, OwnerOfTemp);
	QaDSSchemaUtils::AddAction<UDialogSubGraphEdGraphNode>(TEXT("Add Sub Dialog"), TEXT("Add sub dialog node"), Actions, OwnerOfTemp);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
		ContextMenuBuilder.AddAction(Action);
}

const FPinConnectionResponse UDialogGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	UDialogEdGraphNode* ABase = Cast<UDialogEdGraphNode>(A->GetOwningNode());
	UDialogEdGraphNode* BBase = Cast<UDialogEdGraphNode>(B->GetOwningNode());

	if (A->Direction == B->Direction)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));

	if (A->PinType.PinCategory != B->PinType.PinCategory)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));

	if (ABase->GetUniqueID() == BBase->GetUniqueID())
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same node"));

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

class FConnectionDrawingPolicy* UDialogGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FRectConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UDialogGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::Yellow;
}

bool UDialogGraphSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return false;
}

void UDialogGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Paste);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().SelectAll);

	if(!Cast<UDialogRootEdGraphNode>(InGraphNode))
		MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);

	if (InGraphPin)
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
	else
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

#undef LOCTEXT_NAMESPACE