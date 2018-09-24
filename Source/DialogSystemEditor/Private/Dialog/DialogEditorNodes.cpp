#include "DialogSystemEditor.h"
#include "DialogEditorNodes.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "QaDSSettings.h"
#include "DialogAsset.h"
#include "XmlFile.h"

//FDialogPhraseEvent..........................................................................................................
FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FDialogPhraseEvent>& tuple)
{
	auto subNode = FXmlWriteNode(tuple.Tag);
	auto& value = tuple.Value;

	subNode.Append("type", (int)value.CallType);
	subNode.Append("event", value.EventName);
	subNode.Append("tag", value.FindTag);
	subNode.Append("command", value.Command);

	if (value.ObjectClass != NULL)
		subNode.Append("class", value.ObjectClass->GetFullName());

	subNode.AppendArray("params", "param", value.Parameters);

	node.Childrens.Add(node);
	return node;
}

//FDialogPhraseCondition..........................................................................................................
FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FDialogPhraseCondition>& tuple)
{
	node << FXmlWriteTuple<FDialogPhraseEvent>(tuple.Tag, tuple.Value);
	node.Childrens.Last().Append("invert", tuple.Value.InvertCondition);

	return node;
}

//PhraseNode..........................................................................................................
UDialogPhraseEdGraphNode::UDialogPhraseEdGraphNode()
{
	Data.Source = EDialogPhraseSource::NPC;
}

void UDialogPhraseEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, NAME_None, FName("Input"));
	CreatePin(EGPD_Output, NAME_None, FName("Output"));
}

FText UDialogPhraseEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Data.Text.IsEmpty())
		return FText::FromString("Phrase");

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
		node.Append("quest", Data.StartQuest.GetLongPackageName());

	node.AppendArray("give_keys", "key", Data.GiveKeys);
	node.AppendArray("remove_keys", "key", Data.RemoveKeys);
	node.AppendArray("check_has_keys", "key", Data.CheckHasKeys);
	node.AppendArray("check_dont_has_keys", "key", Data.CheckDontHasKeys);
	node.AppendArray("actions", "action", Data.Action);
	node.AppendArray("predicates", "predicate", Data.Predicate);
	
	return node;
}

void UDialogPhraseEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	Super::LoadInXml(xmlNode, nodeById);

	auto textTag = xmlNode->FindChildNode("text");
	if (textTag != NULL)
		Data.Text = FText::FromString(textTag->GetContent());

	auto sourceTag = xmlNode->FindChildNode("source");
	if (sourceTag != NULL)
		Data.Source = (EDialogPhraseSource)FCString::Atoi(*sourceTag->GetContent());

	auto questTag = xmlNode->FindChildNode("quest");
	if (questTag != NULL)
		Data.StartQuest = TAssetPtr<UQuestAsset>(questTag->GetContent());

	auto timeTag = xmlNode->FindChildNode("time");
	if (timeTag != NULL)
	{
		Data.AutoTime = false;
		Data.PhraseManualTime = FCString::Atof(*timeTag->GetContent());
	}

	FXmlSerealizeHelper::DeserealizeArray(xmlNode->FindChildNode("give_keys"), Data.GiveKeys);
	FXmlSerealizeHelper::DeserealizeArray(xmlNode->FindChildNode("remove_keys"), Data.RemoveKeys);
	FXmlSerealizeHelper::DeserealizeArray(xmlNode->FindChildNode("check_has_keys"), Data.CheckHasKeys);
	FXmlSerealizeHelper::DeserealizeArray(xmlNode->FindChildNode("check_dont_has_keys"), Data.CheckDontHasKeys);
	FXmlSerealizeHelper::DeserealizeArray(xmlNode->FindChildNode("actions"), Data.Action);
	FXmlSerealizeHelper::DeserealizeArray(xmlNode->FindChildNode("predicates"), Data.Predicate);
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

void UDialogSubGraphEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	Super::LoadInXml(xmlNode, nodeById);

	auto assetTag = xmlNode->FindChildNode("asset");
	if (assetTag != NULL)
		TargetDialogAsset = TAssetPtr<UDialogAsset>(assetTag->GetContent());
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

void UDialogElseIfEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	auto xTag = xmlNode->FindChildNode("x");
	if (xTag != NULL)
		NodePosX = FCString::Atoi(*xTag->GetContent());
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

void UDialogRootEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById)
{
	auto xTag = xmlNode->FindChildNode("x");
	if (xTag != NULL)
		NodePosX = FCString::Atoi(*xTag->GetContent());
}

FText UDialogRootEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Start ") + Cast<UDialogAsset>(GetGraph()->GetOuter())->Name.ToString());
}

bool UDialogRootEdGraphNode::CanUserDeleteNode() const
{
	return false;
}
