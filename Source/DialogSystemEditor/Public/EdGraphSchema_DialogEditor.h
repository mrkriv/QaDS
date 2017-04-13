#pragma once
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_DialogEditor.generated.h"

USTRUCT()
struct FDialogSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
	class UEdGraphNode* NodeTemplate;

	FDialogSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, NodeTemplate(NULL)
	{}

	FDialogSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
		, NodeTemplate(NULL)
	{}

	// FEdGraphSchemaAction interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FEdGraphSchemaAction interface

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location, bool bSelectNewNode = true)
	{
		FDialogSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
	}
};

class DIALOGSYSTEMEDITOR_API DialogSchemaUtils
{
public:
	template<typename T>
	static void AddAction(FString Title, FString Tooltip, TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, UEdGraph* OwnerOfTemporaries)
	{
		const FText MenuDesc = FText::FromString(Title);
		const FText Category = FText::FromString(TEXT("Nodes"));
		TSharedPtr<FDialogSchemaAction_NewNode> NewActorNodeAction = AddNewNodeAction(OutActions, Category, MenuDesc, Tooltip);
		T* ActorNode = NewObject<T>((UObject *)OwnerOfTemporaries);
		NewActorNodeAction->NodeTemplate = ActorNode;
	}

	static TSharedPtr<FDialogSchemaAction_NewNode> AddNewNodeAction(TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, const FText& Category, const FText& MenuDesc, const FString& Tooltip)
	{
		TSharedPtr<FDialogSchemaAction_NewNode> NewAction = TSharedPtr<FDialogSchemaAction_NewNode>(new FDialogSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
		OutActions.Add(NewAction);
		return NewAction;
	}
};


UCLASS()
class UEdGraphSchema_DialogEditor : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()
	// Begin EdGraphSchema interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	// End EdGraphSchema interface
};
