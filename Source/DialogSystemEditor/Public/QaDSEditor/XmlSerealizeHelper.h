#pragma once

#include "DialogNodes.h"

class FXmlNode;
class UQaDSEdGraphNode;

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

	template<typename T>
	void Append(FString tag, T value)
	{
		*this << FXmlWriteTuple<T>(tag, value);
	}

	template<typename T>
	void AppendArray(FString tag, FString itemTag, TArray<T> values)
	{
		if (values.Num() != 0)
			return;

		auto node = FXmlWriteNode(tag);
		for (auto& value : values)
		{
			node.Append(itemTag, value);
		}

		Childrens.Add(node);
	}
};

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FString>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value));
	return node;
}

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FName>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value.ToString()));
	return node;
}

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FText>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value.ToString()));
	return node;
}

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<int>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, FString::FromInt(tuple.Value)));
	return node;
}

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<float>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, FString::SanitizeFloat(tuple.Value)));
	return node;
}

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<bool>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value ? "true" : "false"));
	return node;
}

class DIALOGSYSTEMEDITOR_API FXmlSerealizeHelper //todo:: remove this
{
public:
	static FString EncodeString(const FString& input);

	static void DeserealizeArray(FXmlNode* tag, TArray<FString>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FName>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseEvent>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseCondition>& outValues);
	static void Deserealize(FXmlNode* tag, FDialogPhraseEvent& outValue);
	static void Deserealize(FXmlNode* tag, FDialogPhraseCondition& outValue);
};