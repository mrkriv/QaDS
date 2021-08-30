#include "QaDSEdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "XmlSerealizeHelper.h"

TArray<UQaDSEdGraphNode*> UQaDSEdGraphNode::GetChildNodes() const
{
	TArray<UQaDSEdGraphNode*> ChildNodes;

	for (auto Pin : Pins)
	{
		if (Pin->Direction != EGPD_Output)
			continue;

		for (int i = 0; i < Pin->LinkedTo.Num(); i++)
		{
			if (Pin->LinkedTo[i])
				ChildNodes.Add((UQaDSEdGraphNode*)Pin->LinkedTo[i]->GetOwningNode());
		}
	}

	return ChildNodes;
}

int UQaDSEdGraphNode::GetOrder() const
{
	auto inputPin = Pins.FindByPredicate([](const UEdGraphPin* pin) {return pin->Direction == EGPD_Input; });

	if (inputPin == NULL || (*inputPin)->LinkedTo.Num() == 0)
		return 0;

	auto bigOwner = (UQaDSEdGraphNode*)(*inputPin)->LinkedTo[0]->GetOwningNode();
	for (auto ownerPin : (*inputPin)->LinkedTo)
	{
		auto owner = (UQaDSEdGraphNode*)ownerPin->GetOwningNode();

		if (owner != NULL && owner->GetChildNodes().Num() > bigOwner->GetChildNodes().Num())
		{
			bigOwner = owner;
		}
	}

	auto lessCount = 0;
	for (auto node : bigOwner->GetChildNodes())
	{
		if (node->NodePosX < NodePosX)
			lessCount++;
	}

	return lessCount + 1;
}

void UQaDSEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyObserver.IsValid())
	{
		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		PropertyObserver->OnPropertyChanged(this, PropertyName);
	}

	Super::PostEditChangeProperty(e);
}

FXmlWriteNode UQaDSEdGraphNode::SaveToXml() const
{
	auto node = FXmlWriteNode("node");

	node.Append("id", NodeGuid.ToString());
	node.Append("class", GetClass()->GetFName());
	node.Append("x", NodePosX);
	node.Append("y", NodePosY);

	node.Append("links", GetChildNodes());

	return node;
}

void UQaDSEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	reader->TryGet("x", NodePosX);
	reader->TryGet("y", NodePosY);

	FGuid::Parse(reader->Get<FString>("id"), NodeGuid);

	auto links = reader->Get<TArray<FString>>("links");
	for (auto id : links)
	{
		if (!nodeById.Contains(id))
			continue;

		auto node = nodeById[id];
		auto inputPin = node->Pins.FindByPredicate([](UEdGraphPin* pin) { return pin->Direction == EEdGraphPinDirection::EGPD_Input; });
		auto outputPin = Pins.FindByPredicate([](UEdGraphPin* pin) { return pin->Direction == EEdGraphPinDirection::EGPD_Output; });

		if (inputPin != NULL && outputPin != NULL)
			(*outputPin)->MakeLinkTo(*inputPin);
	}
}