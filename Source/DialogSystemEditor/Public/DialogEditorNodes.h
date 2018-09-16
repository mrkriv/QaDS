// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "EdGraph/EdGraphNode.h"
#include "DialogPhrase.h"
#include "DialogEditorNodes.generated.h"

class DIALOGSYSTEMEDITOR_API FNodePropertyObserver
{
public:
	virtual void OnPropertyChanged(class UEdGraphNode* Sender, const FName& PropertyName) = 0;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDdialogEdGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()
		
	class UEdGraphPin* InputPin;
	class UEdGraphPin* OutputPin;

	UPROPERTY()
	UDialogNode* CompileNode;

	TArray<UDdialogEdGraphNode*> GetChildNodes();
	virtual int GetOrder() const;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	TSharedPtr<FNodePropertyObserver> PropertyObserver;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogRootEdGraphNode : public UDdialogEdGraphNode
{
	GENERATED_UCLASS_BODY()

	virtual void AllocateDefaultPins() override;
	virtual bool CanUserDeleteNode() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogPhraseEdGraphNode : public UDdialogEdGraphNode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	UDialogPhraseNode* CompilePhrase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogPhraseInfo Data;

	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogPhraseEdGraphNode_Player : public UDialogPhraseEdGraphNode
{
	GENERATED_UCLASS_BODY()
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogSubGraphEdGraphNode : public UDdialogEdGraphNode
{
	GENERATED_UCLASS_BODY()

};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogElseIfEdGraphNode : public UDdialogEdGraphNode
{
	GENERATED_UCLASS_BODY()
};