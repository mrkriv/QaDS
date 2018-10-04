#pragma once

#include "EdGraph/EdGraphNode.h"
#include "XmlSerealizeHelper.h"
#include "QaDSEdGraphNode.generated.h"

class UEdGraphPin;
class FXmlNode;

class DIALOGSYSTEMEDITOR_API FNodePropertyObserver
{
public:
	virtual void OnPropertyChanged(class UEdGraphNode* Sender, const FName& PropertyName) = 0;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UQaDSEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	TSharedPtr<FNodePropertyObserver> PropertyObserver;
	bool bIsCompile;

	virtual bool IsCompile() { return bIsCompile; }
	virtual void ResetCompile() { bIsCompile = false; }
	virtual void SetCompile() { bIsCompile = true; }

	//virtual UQaDSEdGraphNode* Compile(UQaDSEdGraphNode* owner) {} //todo:: compile in virtual methods
	virtual int GetOrder() const;
	TArray<UQaDSEdGraphNode*> GetChildNodes() const;

	virtual FXmlWriteNode SaveToXml() const;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById);

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
};

FORCEINLINE FXmlWriteNode& operator<<(FXmlWriteNode& node, const FXmlWriteTuple<UQaDSEdGraphNode*>& tuple)
{
	node.Childrens.Add(FXmlWriteNode(tuple.Tag, tuple.Value->NodeGuid.ToString()));
	return node;
}