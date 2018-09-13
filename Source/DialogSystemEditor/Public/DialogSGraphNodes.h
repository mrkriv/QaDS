// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "SGraphNode.h"
#include "DialogEditorNodes.h"
#include "AssetThumbnail.h"
#include "SGraphPin.h"

//PineNode....................................................................................
class SDialogOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogOutputPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual FSlateColor GetPinColor() const override;
	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;
};

//DialogNodeBase.............................................................................................................
class SGraphNode_DialogNodeBase : public SGraphNode, public FNodePropertyObserver
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_DialogNodeBase) { }
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UDialogNodeEditorBase* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreateNodeWidget();
	// End of SGraphNode interface

	// FPropertyObserver interface
	virtual void OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName) override;
	// End of FPropertyObserver interface

protected:
	UDialogNodeEditorBase* NodeBace;
	TSharedPtr<SHorizontalBox> OutputPinBox;
	TSharedPtr<SHorizontalBox> InputPinBox;
	TSharedPtr<SVerticalBox> EventsBox;
	TSharedPtr<SVerticalBox> ConditionsBox;
	TSharedPtr<STextBlock> NodeWiget;
	TSharedPtr<SImage> NodeIcon;

	virtual FName GetIcon() const;
	virtual FReply OnClickedIcon();
	void AddTextToContent(TSharedPtr<SVerticalBox> Container, FString Text, FColor Color);
};

//RootNode.............................................................................................................
class SGraphNode_Root : public SGraphNode_DialogNodeBase
{
public:
	SGraphNode_Root();

protected:
	virtual FName GetIcon() const override;
};

//PhraseNode..................................................................................................................
class SGraphNode_Phrase : public SGraphNode_DialogNodeBase
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_Phrase) { }
	SLATE_END_ARGS()

	UPhraseNode* PhraseNode = 0;

	SGraphNode_Phrase();
	virtual void Construct(const FArguments& InArgs, UPhraseNode* InNode);
	virtual void CreateNodeWidget() override;

	// FPropertyObserver interface
	virtual void OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName) override;
	// End of FPropertyObserver interface

protected:
	TSharedPtr<SWidget> IndexOverlay;
	virtual FName GetIcon() const override;
	virtual FReply OnClickedIcon() override;
};