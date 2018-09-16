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

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual FSlateColor GetPinColor() const override;
	const FSlateBrush* GetPinBorder() const;
};

//DialogNodeBase.............................................................................................................
class SGraphNode_DialogNodeBase : public SGraphNode, public FNodePropertyObserver
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_DialogNodeBase) { }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UDdialogEdGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreateNodeWidget();
	virtual void OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName) override;

	UDdialogEdGraphNode* NodeBace;
	TSharedPtr<SHorizontalBox> OutputPinBox;
	TSharedPtr<SHorizontalBox> InputPinBox;
	TSharedPtr<SVerticalBox> EventsBox;
	TSharedPtr<SVerticalBox> ConditionsBox;
	TSharedPtr<STextBlock> NodeWiget;
	TSharedPtr<SImage> NodeIcon;

	virtual FName GetIcon() const { return ""; }
	virtual FReply OnClickedIcon();
	void AddTextToContent(TSharedPtr<SVerticalBox> Container, FString Text, FColor Color);
};

//RootNode.............................................................................................................
class SGraphNode_Root : public SGraphNode_DialogNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.Root"; }
};

//PhraseNode..................................................................................................................
class SGraphNode_Phrase : public SGraphNode_DialogNodeBase
{
public:
	SLATE_BEGIN_ARGS(SGraphNode_Phrase) { }
	SLATE_END_ARGS()

	UDialogPhraseEdGraphNode* PhraseNode = 0;

	virtual void Construct(const FArguments& InArgs, UDialogPhraseEdGraphNode* InNode);
	virtual void CreateNodeWidget() override;
	virtual void OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName) override;

	TSharedPtr<SWidget> IndexOverlay;
	virtual FName GetIcon() const override;
	virtual FReply OnClickedIcon() override;
};

//SubGraph.............................................................................................................
class SGraphNode_SubGraph : public SGraphNode_DialogNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.SubGraph"; }
};

//ElseIf.............................................................................................................
class SGraphNode_ElseIf : public SGraphNode_DialogNodeBase
{
public:
	virtual FName GetIcon() const override { return "DialogSystem.ElseIf"; }
};