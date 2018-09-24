#include "DialogSystemEditor.h"
#include "XmlSerealizeHelper.h"
#include "QaDSEdGraphNode.h"
#include "XmlFile.h"

FString FXmlWriteNode::GetXml() const
{
	auto header = "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n";
	return header + GetXml("");
}

FString FXmlWriteNode::GetXml(const FString& tab) const
{
	FString xml;

	if (Childrens.Num() > 0)
	{
		xml = tab + "<" + Tag + ">\n";

		for (auto child : Childrens)
		{
			xml += child.GetXml(tab + '\t');
		}

		xml += tab + "</" + Tag + ">\n";
	}
	else if (!Content.IsEmpty())
	{
		auto content = FXmlSerealizeHelper::EncodeString(Content);
		xml += tab + "<" + Tag + ">" + content + "</" + Tag + ">\n";
	}
	else
	{
		xml += tab + "<" + Tag + "/>\n";
	}

	return xml;
}

FString FXmlSerealizeHelper::EncodeString(const FString& input)
{
	FString result;
	result = input; // it work? 
	result.Reset();

	for (auto pos = 0; pos < input.Len(); ++pos)
	{
		auto c = input[pos];
		switch (c)
		{
		case '&':  result += "&amp;";       break;
		case '\"': result += "&quot;";      break;
		case '\'': result += "&apos;";      break;
		case '<':  result += "&lt;";        break;
		case '>':  result += "&gt;";        break;
		default:   result += c; break;
		}
	}

	return result;
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