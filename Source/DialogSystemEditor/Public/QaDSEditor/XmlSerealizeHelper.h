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

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FString>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value));
	return node;
}

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

	void AppendArray(FString tag, FString itemTag, TArray<FString> values);

	template<typename T>
	void Append(FString tag, T value)
	{
		this << FXmlWriteTuple<T>(tag, value);
	}

	void AppendArray(FString tag, FString itemTag, TArray<FString> values);
	void AppendArray(FString tag, FString itemTag, TArray<FName> values);
	void AppendArray(FString tag, FString itemTag, TArray<UQaDSEdGraphNode*> values); // todo:: move to UQaDSEdGraphNode
	void AppendArray(FString tag, FString itemTag, TArray<FDialogPhraseEvent> values); // todo:: move to FDialogPhraseEvent
	void AppendArray(FString tag, FString itemTag, TArray<FDialogPhraseCondition> values); // todo:: move to FDialogPhraseCondition
	void Append(FString tag, const FDialogPhraseEvent& value); // todo:: move to FDialogPhraseEvent
	void Append(FString tag, const FDialogPhraseCondition& value); // todo:: move to FDialogPhraseCondition
	void Append(FString tag, const FString& value);
	void Append(FString tag, const FName& value);
	void Append(FString tag, const FText& value);
	void Append(FString tag, float value);
	void Append(FString tag, int value);
	void Append(FString tag, bool value);
};

class DIALOGSYSTEMEDITOR_API FXmlSerealizeHelper
{
public:
	static FString EncodeString(const FString& input);

	static void DeserealizeArray(FXmlNode* tag, TArray<FString>& outValues); //todo::
	static void DeserealizeArray(FXmlNode* tag, TArray<FName>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseEvent>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseCondition>& outValues);
	static void Deserealize(FXmlNode* tag, FDialogPhraseEvent& outValue);
	static void Deserealize(FXmlNode* tag, FDialogPhraseCondition& outValue);
};