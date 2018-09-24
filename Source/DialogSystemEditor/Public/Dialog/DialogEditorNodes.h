#pragma once

#include "EdGraph/EdGraphNode.h"
#include "DialogNodes.h"
#include "QaDSEdGraphNode.h"
#include "DialogEditorNodes.generated.h"

class UEdGraphPin;
class FXmlNode;

FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FDialogPhraseEvent>& tuple);
FORCEINLINE void operator<<(FXmlWriteNode& node, const FXmlWriteTuple<FDialogPhraseCondition>& tuple);
FORCEINLINE void operator>>(const FXmlReadNode& node, FDialogPhraseEvent& value);
FORCEINLINE void operator>>(const FXmlReadNode& node, FDialogPhraseCondition& value);

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogEdGraphNode : public UQaDSEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UDialogNode* CompileNode;

	virtual UObject* GetCompile() override { return CompileNode; }
	virtual void ResetCompile() override { CompileNode = NULL; }
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogRootEdGraphNode : public UDialogEdGraphNode
{
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual FXmlWriteNode SaveToXml() const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogPhraseEdGraphNode : public UDialogEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogPhraseInfo Data;

	UDialogPhraseEdGraphNode();
	virtual void AllocateDefaultPins() override;
	virtual FXmlWriteNode SaveToXml() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogPhraseEdGraphNode_Player : public UDialogPhraseEdGraphNode
{
	GENERATED_BODY()

public:
	UDialogPhraseEdGraphNode_Player();
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogSubGraphEdGraphNode : public UDialogEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UDialogAsset> TargetDialogAsset;

	virtual void AllocateDefaultPins() override;
	virtual FXmlWriteNode SaveToXml() const override;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogElseIfEdGraphNode : public UDialogEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialogElseIfCondition> Conditions;

	virtual void AllocateDefaultPins() override;
	virtual FXmlWriteNode SaveToXml() const override;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};