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
		auto content = EncodeString(Content);
		xml += tab + "<" + Tag + ">" + content + "</" + Tag + ">\n";
	}
	else
	{
		xml += tab + "<" + Tag + "/>\n";
	}

	return xml;
}

FString FXmlWriteNode::EncodeString(const FString& input)
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

/* Serealize */

void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FString>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value));
}

void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FName>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value.ToString()));
}

void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FText>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value.ToString()));
}

void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<int>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, FString::FromInt(tuple.Value)));
}

void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<float>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, FString::SanitizeFloat(tuple.Value)));
}

void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<bool>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value ? "true" : "false"));
}

/* Deserealize */

void operator>>(const FXmlReadNode& node, FString& value)
{
	value = node.XmlNode->GetContent();
}

void operator>>(const FXmlReadNode& node, FName& value)
{
	value = *node.XmlNode->GetContent();
}

void operator>>(const FXmlReadNode& node, FText& value)
{
	value = FText::FromString(node.XmlNode->GetContent());
}

void operator>>(const FXmlReadNode& node, int& value)
{
	value = FCString::Atoi(*node.XmlNode->GetContent());
}

void operator>>(const FXmlReadNode& node, float& value)
{
	value = FCString::Atof(*node.XmlNode->GetContent());
}

void operator>>(const FXmlReadNode& node, bool& value)
{
	value = node.XmlNode->GetContent().ToLower() == "true";
}

void operator>>(const FXmlReadNode& node, UClass*& value)
{
	value = FindObject<UClass>(ANY_PACKAGE, *node.XmlNode->GetContent());
}
