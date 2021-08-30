#include "QaDSGraphSchema.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"

UEdGraphNode* FQaDSSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UEdGraphNode* ResultNode = NULL;

	if (NodeTemplate != NULL)
	{
		NodeTemplate->SetFlags(RF_Transactional);

		NodeTemplate->Rename(NULL, (UObject *)ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();

		if (FromPin != NULL)
		{
			auto Schema = ParentGraph->GetSchema();

			for (auto pin : NodeTemplate->GetAllPins())
			{
				if (Schema->TryCreateConnection(FromPin, pin))
				{
					FromPin->GetOwningNode()->NodeConnectionListChanged();
					break;
				}
				else if (Schema->TryCreateConnection(pin, FromPin))
				{
					NodeTemplate->NodeConnectionListChanged();
					break;
				}
			}
		}

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(16);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

UEdGraphNode* FQaDSSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UEdGraphNode* ResultNode = NULL;

	if (FromPins.Num() > 0)
	{
		ResultNode = PerformAction(ParentGraph, FromPins[0], Location, bSelectNewNode);

		for (int32 Index = 1; Index < FromPins.Num(); ++Index)
			ResultNode->AutowireNewNode(FromPins[Index]);
	}
	else
		ResultNode = PerformAction(ParentGraph, NULL, Location, bSelectNewNode);

	return ResultNode;
}

void FQaDSSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}