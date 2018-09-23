#include "DialogSystemEditor.h"
#include "DialogEditorNodes.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "QaDSSettings.h"
#include "DialogAsset.h"
#include "XmlSerealizeHelper.h"
#include "XmlFile.h"

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

FString UDialogPhraseEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	xml += tab + "<text>" + Data.Text.ToString() + "</text>\n";
	xml += tab + "<source>" + FString::FromInt((uint8)Data.Source) + "</source>\n";

	if (!Data.AutoTime)
		xml += tab + "<time>" + FString::SanitizeFloat(Data.PhraseManualTime) + "</time>\n";

	if (Data.StartQuest.IsValid())
		xml += tab + "<quest>" + Data.StartQuest.GetLongPackageName() + "</quest>\n";

	xml += FXmlSerealizeHelper::SerealizeArray(tab, "give_keys", "key", Data.GiveKeys);
	xml += FXmlSerealizeHelper::SerealizeArray(tab, "remove_keys", "key", Data.RemoveKeys);
	xml += FXmlSerealizeHelper::SerealizeArray(tab, "check_has_keys", "key", Data.CheckHasKeys);
	xml += FXmlSerealizeHelper::SerealizeArray(tab, "check_dont_has_keys", "key", Data.CheckDontHasKeys);
	xml += FXmlSerealizeHelper::SerealizeArray(tab, "actions", "action", Data.Action);
	xml += FXmlSerealizeHelper::SerealizeArray(tab, "predicates", "predicate", Data.Predicate);
	
	return xml;
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

FString UDialogSubGraphEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	xml += tab + "<asset>" + TargetDialogAsset.GetLongPackageName() + "</asset>\n";

	return xml;
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

FString UDialogElseIfEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	return xml;
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

FString UDialogRootEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	return xml;
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
