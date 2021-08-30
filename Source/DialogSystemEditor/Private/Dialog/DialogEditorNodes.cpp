#include "DialogEditorNodes.h"
#include "EdGraph/EdGraph.h"
#include "DialogAsset.h"

//PhraseNode..........................................................................................................
UDialogPhraseEdGraphNode::UDialogPhraseEdGraphNode()
{
	Data.Source = EDialogPhraseSource::NPC;
}

void UDialogPhraseEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
	CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FText UDialogPhraseEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Data.Text.IsEmpty())
		return FText::FromString("Proxy node");

	return Data.Text;
}

FXmlWriteNode UDialogPhraseEdGraphNode::SaveToXml() const
{
	auto node = Super::SaveToXml();

	node.Append("text", Data.Text.ToString());
	node.Append("source", (int)Data.Source);

	if (!Data.AutoTime)
		node.Append("time", Data.PhraseManualTime);

	if (Data.StartQuest.IsValid())
		node.Append("quest", Data.StartQuest.ToSoftObjectPath().ToString());

	node.Append("aditionals", Data.AditionalData);
	node.Append("give_keys", Data.GiveKeys);
	node.Append("remove_keys", Data.RemoveKeys);
	node.Append("check_has_keys", Data.CheckHasKeys);
	node.Append("check_dont_has_keys", Data.CheckDontHasKeys);
	node.Append("actions", Data.Action);
	node.Append("predicates", Data.Predicate);
	
	return node;
}

void UDialogPhraseEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	Super::LoadInXml(reader, nodeById);

	reader->TryGet("text", Data.Text);
	reader->TryGet("source", (int&)Data.Source);
	reader->TryGet("time", Data.PhraseManualTime);

	reader->TryGet("aditionals", Data.AditionalData);
	reader->TryGet("give_keys", Data.GiveKeys);
	reader->TryGet("remove_keys", Data.RemoveKeys);
	reader->TryGet("check_has_keys", Data.CheckHasKeys);
	reader->TryGet("check_dont_has_keys", Data.CheckDontHasKeys);
	reader->TryGet("give_actionskeys", Data.Action);
	reader->TryGet("predicates", Data.Predicate);

	Data.AutoTime = Data.PhraseManualTime > 0;
	Data.StartQuest = TAssetPtr<UQuestAsset>(reader->Get("quest"));
}

void UDialogPhraseEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UDialogEdGraphNode::PostEditChangeProperty(e);
}

//UDialogSubGraphEdGraphNode...........................................................................................
void UDialogSubGraphEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
}

FXmlWriteNode UDialogSubGraphEdGraphNode::SaveToXml() const
{
	auto node = Super::SaveToXml();
	node.Append("asset", TargetDialogAsset.GetLongPackageName());

	return node;
}

void UDialogSubGraphEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	Super::LoadInXml(reader, nodeById);
	TargetDialogAsset = TAssetPtr<UDialogAsset>(reader->Get("asset"));
}

FText UDialogSubGraphEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetDialogAsset != NULL)
		return FText::FromString(TargetDialogAsset.GetAssetName());

	return FText::FromString("Asset is not set!");
}

//UDialogElseIfEdGraphNode...........................................................................................

void UDialogElseIfEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
	CreatePin(EGPD_Output, NAME_None, FName("Default"));
}

FXmlWriteNode UDialogElseIfEdGraphNode::SaveToXml() const
{
	auto node = Super::SaveToXml();
	return node;
}

void UDialogElseIfEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
}

FText UDialogElseIfEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Else-if");
}

//UDialogRootEdGraphNode...........................................................................................

UDialogPhraseEdGraphNode_Player::UDialogPhraseEdGraphNode_Player()
{
	Data.Source = EDialogPhraseSource::Player;
}

//UDialogRootEdGraphNode...........................................................................................
void UDialogRootEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	CreatePin(EGPD_Output, NAME_None, FName("Start"));
}

FXmlWriteNode UDialogRootEdGraphNode::SaveToXml() const
{
	auto node = Super::SaveToXml();
	return node;
}

void UDialogRootEdGraphNode::LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	Super::LoadInXml(reader, nodeById);
}

FText UDialogRootEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Start ") + Cast<UDialogAsset>(GetGraph()->GetOuter())->Name.ToString());
}

bool UDialogRootEdGraphNode::CanUserDeleteNode() const
{
	return false;
}

//FDialogPhraseEvent..........................................................................................................
void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FDialogPhraseEvent>& tuple)
{
	auto subNode = FXmlWriteNode(tuple.Tag);
	auto& value = tuple.Value;

	subNode.Append("type", (int)value.CallType);
	subNode.Append("event", value.EventName);
	subNode.Append("tag", value.FindTag);
	subNode.Append("command", value.Command);

	if (value.ObjectClass != NULL)
		subNode.Append("class", value.ObjectClass->GetFullName());

	subNode.Append("params", value.Parameters);

	node.Childrens.Add(subNode);
}

void operator>>(const FXmlReadNode& node, FDialogPhraseEvent& value)
{
	node.TryGet("type", (int&)value.CallType);
	node.TryGet("tag", value.FindTag);
	node.TryGet("command", value.Command);
	node.TryGet("params", value.Parameters);

	value.ObjectClass = FindObject<UClass>(ANY_PACKAGE, *node.Get("class"));
}

//FDialogPhraseCondition..........................................................................................................
void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FDialogPhraseCondition>& tuple)
{
	node << FXmlWriteTuple<FDialogPhraseEvent>(tuple.Tag, tuple.Value);
	node.Childrens.Last().Append("invert", tuple.Value.InvertCondition);
}

void operator>>(const FXmlReadNode& node, FDialogPhraseCondition& value)
{
	node >> (FDialogPhraseEvent&)value;
	node.TryGet("invert", value.InvertCondition);
}
