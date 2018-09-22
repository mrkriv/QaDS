#include "DialogSystemEditor.h"
#include "XmlSerealizeHelper.h"
#include "XmlFile.h"

FString FXmlSerealizeHelper::SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FString> values)
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

FString FXmlSerealizeHelper::SerealizeArray(FString tab, FString tag, FString itemTag, TArray<UQaDSEdGraphNode*> values)
{
	TArray<FString> newValues;

	for (auto value : values)
		newValues.Add(value->NodeGuid.ToString());

	return FXmlSerealizeHelper::SerealizeArray(tab, tag, itemTag, newValues);
}

FString FXmlSerealizeHelper::SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FName> values)
{
	TArray<FString> newValues;

	for (auto& value : values)
		newValues.Add(value.ToString());

	return FXmlSerealizeHelper::SerealizeArray(tab, tag, itemTag, newValues);
}

FString FXmlSerealizeHelper::Serealize(FString tab, const FDialogPhraseEvent& value)
{
	FString xml = "";
	xml += tab + "\t<type>" + FString::FromInt((uint8)value.CallType) + "</type>\n";
	xml += tab + "\t<event>" + value.EventName.ToString() + "</event>\n";
	xml += tab + "\t<tag>" + value.FindTag + "</tag>\n";
	xml += tab + "\t<command>" + value.Command + "</command>\n";

	if (value.ObjectClass != NULL)
		xml += tab + "\t<class>" + value.ObjectClass->GetFullName() + "</class>\n";

	xml += FXmlSerealizeHelper::SerealizeArray(tab + "\n", "params", "param", value.Parameters);

	return xml;
}

FString FXmlSerealizeHelper::Serealize(FString tab, const FDialogPhraseCondition& value)
{
	FString xml = "";
	xml += tab + "\t<invert>" + (value.InvertCondition ? "true" : "false") + "</invert>\n";
	xml += Serealize(tab, (FDialogPhraseEvent)value);

	return xml;
}

FString FXmlSerealizeHelper::SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseEvent> values)
{
	TArray<FString> newValues;

	for (auto& value : values)
	{
		newValues.Add("\n" + Serealize(tab + '\t', value) + tab + '\t');
	}

	return FXmlSerealizeHelper::SerealizeArray(tab, tag, itemTag, newValues);
}

FString FXmlSerealizeHelper::SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseCondition> values)
{
	TArray<FString> newValues;

	for (auto& value : values)
	{
		newValues.Add("\n" + Serealize(tab + '\t', value) + tab + '\t');
	}

	return FXmlSerealizeHelper::SerealizeArray(tab, tag, itemTag, newValues);
}

void FXmlSerealizeHelper::DeserealizeArray(FXmlNode* tag, TArray<FString>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		outValues.Add(childTag->GetContent());
	}
}

void FXmlSerealizeHelper::DeserealizeArray(FXmlNode* tag, TArray<FName>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		outValues.Add(*childTag->GetContent());
	}
}

void FXmlSerealizeHelper::DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseEvent>& outValues)
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

void FXmlSerealizeHelper::DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseCondition>& outValues)
{
	outValues.Reset();

	if (tag == NULL)
		return;

	for (auto childTag : tag->GetChildrenNodes())
	{
		FDialogPhraseCondition out;
		FXmlSerealizeHelper::Deserealize(childTag, out);
		outValues.Add(out);
	}
}

void FXmlSerealizeHelper::Deserealize(FXmlNode* tag, FDialogPhraseEvent& outValue)
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

	FXmlSerealizeHelper::DeserealizeArray(tag->FindChildNode("params"), outValue.Parameters);
}

void FXmlSerealizeHelper::Deserealize(FXmlNode* tag, FDialogPhraseCondition& outValue)
{
	FXmlSerealizeHelper::Deserealize(tag, (FDialogPhraseEvent&)outValue);

	auto invertTag = tag->FindChildNode("invert");
	if (invertTag != NULL)
		outValue.InvertCondition = invertTag->GetContent().ToLower() == "true";
}