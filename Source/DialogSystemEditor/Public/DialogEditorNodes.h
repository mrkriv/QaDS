// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "EdGraph/EdGraphNode.h"
#include "DialogPhrase.h"
#include "DialogEditorNodes.generated.h"

struct FPinDataTypes
{
	static const FString PinType_Root;
};

class DIALOGSYSTEMEDITOR_API FNodePropertyObserver
{
public:
	virtual void OnPropertyChanged(class UEdGraphNode* Sender, const FName& PropertyName) = 0;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogNodeEditorBase : public UEdGraphNode
{
	GENERATED_BODY()
public:

	virtual TArray<UDialogNodeEditorBase*> GetChildNodes();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	TSharedPtr<FNodePropertyObserver> PropertyObserver;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UPhraseNode : public UDialogNodeEditorBase
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY()
	UDialogPhrase* CompilePhrase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDialogPhraseInfo Data;

	class UEdGraphPin* InputPin;
	class UEdGraphPin* OutputPin;

	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UPhrasePlayerNode : public UPhraseNode
{
	GENERATED_UCLASS_BODY()
};

UCLASS()
class DIALOGSYSTEMEDITOR_API URootNode : public UPhraseNode
{
	GENERATED_UCLASS_BODY()
public:

	virtual void AllocateDefaultPins() override;
	virtual bool CanUserDeleteNode() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};
