#include "DialogSystemEditor.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "StoryInformationManager.h"
#include "EdGraph/EdGraph.h"
#include "DialogEditorNodes.h"
#include "DialogSettings.h"
#include "DialogAsset.h"
#include "XmlFile.h"

FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FString> values);
FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<UDialogEdGraphNode*> values);
FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FName> values);
FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseEvent> values);
FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseCondition> values);
FString Serealize(FString tab, const FDialogPhraseEvent& value);
FString Serealize(FString tab, const FDialogPhraseCondition& value);

void DeserealizeArray(FXmlNode* tag, TArray<FString>& outValues);
void DeserealizeArray(FXmlNode* tag, TArray<FName>& outValues);
void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseEvent>& outValues);
void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseCondition>& outValues);
void Deserealize(FXmlNode* tag, FDialogPhraseEvent& outValue);
void Deserealize(FXmlNode* tag, FDialogPhraseCondition& outValue);

//UDialogEdGraphNode...........................................................................................
TArray<UDialogEdGraphNode*> UDialogEdGraphNode::GetChildNodes() const
{
	TArray<UDialogEdGraphNode*> ChildNodes;

	for (auto Pin : Pins)
	{
		if (Pin->Direction != EGPD_Output)
			continue;

		for (int i = 0; i < Pin->LinkedTo.Num(); i++)
		{
			if (Pin->LinkedTo[i])
				ChildNodes.Add((UDialogEdGraphNode*)Pin->LinkedTo[i]->GetOwningNode());
		}
	}

	return ChildNodes;
}

void UDialogEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyObserver.IsValid())
	{
		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		PropertyObserver->OnPropertyChanged(this, PropertyName);
	}

	Super::PostEditChangeProperty(e);
}

int UDialogEdGraphNode::GetOrder() const
{
	auto inputPin = Pins.FindByPredicate([](const UEdGraphPin* pin) {return pin->Direction == EGPD_Input; });

	if(inputPin == NULL || (*inputPin)->LinkedTo.Num() == 0)
		return 0;
	
	auto bigOwner = (UDialogEdGraphNode*)(*inputPin)->LinkedTo[0]->GetOwningNode();
	for(auto ownerPin : (*inputPin)->LinkedTo)
	{
		auto owner = (UDialogEdGraphNode*)ownerPin->GetOwningNode();

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

FString UDialogEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml;

	xml += tab + "<id>" + NodeGuid.ToString() + "</id>\n";
	xml += tab + "<class>" + GetClass()->GetFName().ToString() + "</class>\n";
	xml += tab + "<x>" + FString::FromInt(NodePosX) + "</x>\n";
	xml += tab + "<y>" + FString::FromInt(NodePosY) + "</y>\n";
	xml += SerealizeArray(tab, "links", "link", GetChildNodes());

	return xml;
}

void UDialogEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UDialogEdGraphNode*>& nodeById)
{
	auto xTag = xmlNode->FindChildNode("x");
	if (xTag != NULL)
		NodePosX = FCString::Atoi(*xTag->GetContent());

	auto yTag = xmlNode->FindChildNode("y");
	if (yTag != NULL)
		NodePosY = FCString::Atoi(*yTag->GetContent());

	auto linksTag = xmlNode->FindChildNode("links");
	if (linksTag != NULL)
	{
		for (auto linkTag : linksTag->GetChildrenNodes())
		{
			auto id = linkTag->GetContent();
			auto node = nodeById[id];

			if (node != NULL && node->InputPin != NULL && OutputPin != NULL)
			{
				OutputPin->MakeLinkTo(node->InputPin);
			}
		}
	}
}

//PhraseNode..........................................................................................................
UDialogPhraseEdGraphNode::UDialogPhraseEdGraphNode()
{
	Data.Source = EDialogPhraseSource::NPC;
}

void UDialogPhraseEdGraphNode::AllocateDefaultPins()
{
	InputPin = CreatePin(EGPD_Input, NAME_None, FName("Input"));
	OutputPin = CreatePin(EGPD_Output, NAME_None, FName("Output"));
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

	xml += SerealizeArray(tab, "give_keys", "key", Data.GiveKeys);
	xml += SerealizeArray(tab, "remove_keys", "key", Data.RemoveKeys);
	xml += SerealizeArray(tab, "check_has_keys", "key", Data.CheckHasKeys);
	xml += SerealizeArray(tab, "check_dont_has_keys", "key", Data.CheckDontHasKeys);
	xml += SerealizeArray(tab, "actions", "action", Data.Action);
	xml += SerealizeArray(tab, "predicates", "predicate", Data.Predicate);

	return xml;
}

void UDialogPhraseEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UDialogEdGraphNode*>& nodeById)
{
	Super::LoadInXml(xmlNode, nodeById);

	auto textTag = xmlNode->FindChildNode("text");
	if (textTag != NULL)
		Data.Text = FText::FromString(textTag->GetContent());

	auto sourceTag = xmlNode->FindChildNode("source");
	if (sourceTag != NULL)
		Data.Source = (EDialogPhraseSource)FCString::Atoi(*sourceTag->GetContent());

	auto timeTag = xmlNode->FindChildNode("time");
	if (timeTag != NULL)
	{
		Data.AutoTime = false;
		Data.PhraseManualTime = FCString::Atof(*timeTag->GetContent());
	}

	DeserealizeArray(xmlNode->FindChildNode("give_keys"), Data.GiveKeys);
	DeserealizeArray(xmlNode->FindChildNode("remove_keys"), Data.RemoveKeys);
	DeserealizeArray(xmlNode->FindChildNode("check_has_keys"), Data.CheckHasKeys);
	DeserealizeArray(xmlNode->FindChildNode("check_dont_has_keys"), Data.CheckDontHasKeys);
	DeserealizeArray(xmlNode->FindChildNode("actions"), Data.Action);
	DeserealizeArray(xmlNode->FindChildNode("predicates"), Data.Predicate);
}

void UDialogPhraseEdGraphNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UDialogEdGraphNode::PostEditChangeProperty(e);
}

//UDialogSubGraphEdGraphNode...........................................................................................
void UDialogSubGraphEdGraphNode::AllocateDefaultPins()
{
	Pins.Reset();
	InputPin = CreatePin(EGPD_Input, NAME_None, FName("Input"));
}

FString UDialogSubGraphEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	xml += tab + "<asset>" + TargetDialogAsset.GetLongPackageName() + "</asset>\n";

	return xml;
}

void UDialogSubGraphEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UDialogEdGraphNode*>& nodeById)
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
	InputPin = CreatePin(EGPD_Input, NAME_None, FName("Input"));
	OutputPin = CreatePin(EGPD_Output, NAME_None, FName("Default"));
}

FString UDialogElseIfEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	return xml;
}

void UDialogElseIfEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UDialogEdGraphNode*>& nodeById)
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
	OutputPin = CreatePin(EGPD_Output, NAME_None, FName("Start"));
}

FString UDialogRootEdGraphNode::SaveToXml(int tabLevel) const
{
	FString tab(tabLevel, TEXT("\t\t\t\t\t\t\t\t\t\t\t"));
	FString xml = Super::SaveToXml(tabLevel);

	return xml;
}

void UDialogRootEdGraphNode::LoadInXml(FXmlNode* xmlNode, const TMap<FString, UDialogEdGraphNode*>& nodeById)
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


/*		Serealize and Deserealize from XML		*/

FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FString> values)
{
	if (values.Num() == 0)
		return "";

	FString xml = tab + "<" + tag + ">\n";
	for (auto& value : values)
	{
		xml += tab + "\t<" + itemTag + ">" + value + "</" + itemTag + ">\n";
	}
	xml += tab + "</" + tag + ">\n";

	return xml;
}

FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<UDialogEdGraphNode*> values)
{
	TArray<FString> newValues;

	for (auto value : values)
		newValues.Add(value->NodeGuid.ToString());

	return SerealizeArray(tab, tag, itemTag, newValues);
}

FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FName> values)
{
	TArray<FString> newValues;

	for (auto& value : values)
		newValues.Add(value.ToString());

	return SerealizeArray(tab, tag, itemTag, newValues);
}

FString Serealize(FString tab, const FDialogPhraseEvent& value)
{
	FString xml = "";
	xml += tab + "\t<type>" + FString::FromInt((uint8)value.CallType) + "</type>\n";
	xml += tab + "\t<event>" + value.EventName.ToString() + "</event>\n";
	xml += tab + "\t<tag>" + value.FindTag + "</tag>\n";
	xml += tab + "\t<command>" + value.Command + "</command>\n";

	if (value.ObjectClass != NULL)
		xml += tab + "\t<class>" + value.ObjectClass->GetFullName() + "</class>\n";

	xml += SerealizeArray(tab + "\n", "params", "param", value.Parameters);

	return xml;
}

FString Serealize(FString tab, const FDialogPhraseCondition& value)
{
	FString xml = "";
	xml += tab + "\t<invert>" + (value.InvertCondition ? "true" : "false") + "</invert>\n";
	xml += Serealize(tab, (FDialogPhraseEvent)value);

	return xml;
}

FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseEvent> values)
{
	TArray<FString> newValues;

	for (auto& value : values)
	{
		newValues.Add("\n" + Serealize(tab + '\t', value) + tab + '\t');
	}

	return SerealizeArray(tab, tag, itemTag, newValues);
}

FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseCondition> values)
{
	TArray<FString> newValues;

	for (auto& value : values)
	{
		newValues.Add("\n" + Serealize(tab + '\t', value) + tab + '\t');
	}

	return SerealizeArray(tab, tag, itemTag, newValues);
}

void DeserealizeArray(FXmlNode* tag, TArray<FString>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		outValues.Add(childTag->GetContent());
	}
}

void DeserealizeArray(FXmlNode* tag, TArray<FName>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		outValues.Add(*childTag->GetContent());
	}
}

void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseEvent>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		FDialogPhraseEvent out;
		Deserealize(childTag, out);
		outValues.Add(out);
	}
}

void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseCondition>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		FDialogPhraseCondition out;
		Deserealize(childTag, out);
		outValues.Add(out);
	}
}

void Deserealize(FXmlNode* tag, FDialogPhraseEvent& outValue)
{
	auto typeTag = tag->FindChildNode("type");
	if (typeTag != NULL)
		outValue.CallType = (EDialogPhraseEventCallType)FCString::Atoi(*typeTag->GetContent());

	auto eventTag = tag->FindChildNode("event");
	if (eventTag != NULL)
		outValue.EventName = *eventTag->GetContent();

	auto tagTag = tag->FindChildNode("tag");
	if (tagTag != NULL)
		outValue.FindTag = tagTag->GetContent();

	auto commandTag = tag->FindChildNode("command");
	if (commandTag != NULL)
		outValue.Command = commandTag->GetContent();

	auto classTag = tag->FindChildNode("command");
	if (classTag != NULL)
	{
		auto objClass = FindObject<UClass>(ANY_PACKAGE, *classTag->GetContent());
		outValue.ObjectClass = objClass;
	}

	DeserealizeArray(tag->FindChildNode("params"), outValue.Parameters);
}

void Deserealize(FXmlNode* tag, FDialogPhraseCondition& outValue)
{
	Deserealize(tag, (FDialogPhraseEvent&)outValue);

	auto invertTag = tag->FindChildNode("invert");
	if (invertTag != NULL)
		outValue.InvertCondition = invertTag->GetContent().ToLower() == "true";
}