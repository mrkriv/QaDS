#pragma once

#include "SGraphNode.h"
#include "AssetThumbnail.h"
#include "SGraphPin.h"

//PineNode....................................................................................
class SQaDSOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SQaDSOutputPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual FSlateColor GetPinColor() const override;
	const FSlateBrush* GetPinBorder() const;
};

//NodeBase.............................................................................................................
class SGraphNode_QaDSNodeBase : public SGraphNode, public FNodePropertyObserver
{
	SLATE_BEGIN_ARGS(SGraphNode_QaDSNodeBase) { }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UQaDSEdGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreateNodeWidget();
	virtual void OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName) override;
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime) override;

	TSharedPtr<SHorizontalBox> OutputPinBox;
	TSharedPtr<SHorizontalBox> InputPinBox;
	TSharedPtr<SVerticalBox> ConditionsBox;
	TSharedPtr<SVerticalBox> EventsBox;
	TSharedPtr<SVerticalBox> BodyBox;
	TSharedPtr<STextBlock> NodeWiget;
	TSharedPtr<SImage> NodeIcon;
	TSharedPtr<SBorder> OrderDisplayBorder;
	TSharedPtr<STextBlock> OrderDisplayText;
	
	virtual FName GetIcon() const { return ""; }
	virtual FReply OnClickedIcon();
	void AddTextToContent(TSharedPtr<SVerticalBox> Container, const FString& Prefix, const FString& Text, const FColor& Color);
};
