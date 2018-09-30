#pragma once

#include "XmlFile.h"
#include "DialogNodes.h"

class FXmlNode;

template<typename T>
struct FXmlWriteTuple
{
	FString Tag;
	T Value;

	FXmlWriteTuple(const FString& tag, const T& value) : Tag(tag), Value(value) {}
};

class DIALOGSYSTEMEDITOR_API FXmlWriteNode
{
public:
	FString Tag;
	FString Content;
	TArray<FXmlWriteNode> Childrens;

	FXmlWriteNode() {}
	FXmlWriteNode(const FString& tag) : Tag(tag) {}
	FXmlWriteNode(const FString& tag, const FString& content) : Tag(tag), Content(content) {}

	FString GetXml() const;
	FString GetXml(const FString& tab) const;

	static FString EncodeString(const FString& input);

	template<typename T>
	FORCEINLINE void Append(FString tag, T value)
	{
		*this << FXmlWriteTuple<T>(tag, value);
	}

	template<typename T>
	FORCEINLINE void AppendArray(FString tag, FString itemTag, TArray<T> values)
	{
		if (values.Num() == 0)
			return;

		auto node = FXmlWriteNode(tag);
		for (auto& value : values)
		{
			node.Append(itemTag, value);
		}

		Childrens.Add(node);
	}
};

FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FString>& tuple);
FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FName>& tuple);
FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FText>& tuple);
FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<int>& tuple);
FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<float>& tuple);
FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<bool>& tuple);

class DIALOGSYSTEMEDITOR_API FXmlReadNode
{
public:
	FXmlNode* XmlNode;

	FXmlReadNode() {}
	FXmlReadNode(FXmlNode* xmlNode) : XmlNode(xmlNode) {}

	FORCEINLINE FString Get(const FString& tag) const
	{
		auto xml = XmlNode->FindChildNode(tag);

		if (xml != NULL)
			return xml->GetContent();

		return "";
	}

	template<typename T>
	FORCEINLINE T Get(const FString& tag) const
	{
		T item;
		TryGet(tag, item);

		return item;
	}

	template<typename T>
	FORCEINLINE void TryGet(const FString& tag, T& outValue) const
	{
		auto xml = XmlNode->FindChildNode(tag);

		if (xml != NULL)
			FXmlReadNode(xml) >> outValue;
	}

	template<typename T>
	FORCEINLINE void TryGet(const FString& tag, TArray<T>& outValue) const
	{
		auto xml = XmlNode->FindChildNode(tag);
		outValue.Reset();

		if (xml == NULL)
			return;

		for (auto subXml : xml->GetChildrenNodes())
		{
			T item;
			FXmlReadNode(subXml) >> item;

			outValue.Add(item);
		}
	}
};

FORCEINLINE void operator>>(const FXmlReadNode& node, FString& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, FName& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, FText& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, int& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, float& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, bool& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, UClass*& value);
