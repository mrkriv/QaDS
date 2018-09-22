#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "QaDSGraphSchema.generated.h"

USTRUCT()
struct FQaDSSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY()
	class UEdGraphNode* NodeTemplate;

	FQaDSSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, NodeTemplate(NULL)
	{}

	FQaDSSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FString& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, FText::FromString(InToolTip), InGrouping)
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
		FQaDSSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
	}
};

class DIALOGSYSTEMEDITOR_API QaDSSchemaUtils
{
public:
	template<typename T>
	static void AddAction(FString Title, FString Tooltip, TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, UEdGraph* OwnerOfTemporaries)
	{
		const FText MenuDesc = FText::FromString(Title);
		const FText Category = FText::FromString(TEXT("Nodes"));
		TSharedPtr<FQaDSSchemaAction_NewNode> NewActorNodeAction = AddNewNodeAction(OutActions, Category, MenuDesc, Tooltip);
		T* ActorNode = NewObject<T>((UObject *)OwnerOfTemporaries);
		NewActorNodeAction->NodeTemplate = ActorNode;
	}

	static TSharedPtr<FQaDSSchemaAction_NewNode> AddNewNodeAction(TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, const FText& Category, const FText& MenuDesc, const FString& Tooltip)
	{
		TSharedPtr<FQaDSSchemaAction_NewNode> NewAction = TSharedPtr<FQaDSSchemaAction_NewNode>(new FQaDSSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
		OutActions.Add(NewAction);
		return NewAction;
	}
};