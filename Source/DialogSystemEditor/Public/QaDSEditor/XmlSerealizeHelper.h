#pragma once

#include "DialogNodes.h"

class FXmlNode;

class DIALOGSYSTEMEDITOR_API FXmlSerealizeHelper
{
public:
	static FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FString> values);
	static FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<UQaDSEdGraphNode*> values);
	static FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FName> values);
	static FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseEvent> values);
	static FString SerealizeArray(FString tab, FString tag, FString itemTag, TArray<FDialogPhraseCondition> values);
	static FString Serealize(FString tab, const FDialogPhraseEvent& value);
	static FString Serealize(FString tab, const FDialogPhraseCondition& value);

	static void DeserealizeArray(FXmlNode* tag, TArray<FString>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FName>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseEvent>& outValues);
	static void DeserealizeArray(FXmlNode* tag, TArray<FDialogPhraseCondition>& outValues);
	static void Deserealize(FXmlNode* tag, FDialogPhraseEvent& outValue);
	static void Deserealize(FXmlNode* tag, FDialogPhraseCondition& outValue);
};