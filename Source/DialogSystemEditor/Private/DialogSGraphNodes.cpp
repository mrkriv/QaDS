// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "GraphEditor.h"
#include "SInlineEditableTextBlock.h"
#include "AssetThumbnail.h"
#include "AssetRegistryModule.h"
#include "DialogSGraphNodes.h"
#include "DialogSettings.h"
#include "SNumericEntryBox.h"
#include "GraphEditor.h"
#include "BrushSet.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"

//PinNode..........................................................................
void SDialogOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;
	IsEditable = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SDialogOutputPin::GetPinBorder)
		.BorderBackgroundColor(this, &SDialogOutputPin::GetPinColor)
		.OnMouseButtonDown(this, &SDialogOutputPin::OnPinMouseDown)
		.Cursor(this, &SDialogOutputPin::GetPinCursor)
		.Padding(FMargin(5.0f))
	);
}

TSharedRef<SWidget>	SDialogOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SDialogOutputPin::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("DetailsView.CategoryMiddle"));
}

FSlateColor SDialogOutputPin::GetPinColor() const
{
	FLinearColor defaultColor = FLinearColor(0.9f, 0.9f, 0.9f);

	return IsHovered() ? FLinearColor(0.65f, 0.65f, 0.65f) : defaultColor;
}

//DialogNodeBase............................................................................................................

void SGraphNode_DialogNodeBase::Construct(const FArguments& InArgs, UDialogNodeEditorBase* InNode)
{
	GraphNode = InNode;
	NodeBace = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SGraphNode_DialogNodeBase::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	InputPinBox.Reset();
	RightNodeBox.Reset();
	OutputPinBox.Reset();

	EventsBox.Reset();
	ConditionsBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0.0f, 0.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
				.BorderBackgroundColor(FLinearColor(.7f, .7f, .7f))
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Top)
					.AutoHeight()
					[
						SAssignNew(InputPinBox, SHorizontalBox) // INPUT PIN AREA
					]
	
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Top)
					.Padding(5.0f, 7.0f)
					.AutoHeight()
					[
						SAssignNew(ConditionsBox, SVerticalBox)
					]
				]
			]

			+ SVerticalBox::Slot()
			.Padding(-15.0f, 0.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
			]
	
			+ SVerticalBox::Slot()
			.Padding(-15.0f, 0.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryMiddle"))
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					.Padding(-15.0f, 0.0f, -5.0f, 0.0f)
					.AutoWidth()
					[
						SNew(SButton)
						.OnClicked(this, &SGraphNode_DialogNodeBase::OnClickedIcon)
						.ButtonStyle(FEditorStyle::Get(), "FlatButton")
						[
							SAssignNew(NodeIcon, SImage)
							.Image(FBrushSet::Get().GetBrush(GetIcon()))
						]
					]

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.FillWidth(1.0f)
					.Padding(2.0f, 2.0f, 10.0f, 4.0f)
					[
						SNew(SBox)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.MinDesiredHeight(70.0f)
						.WidthOverride(320.0f)
						.MaxDesiredWidth(320.0f)
						[
							SAssignNew(NodeWiget, STextBlock)
						]
					]
				]
			]

			+ SVerticalBox::Slot()
			.Padding(-15.0f, 0.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryBottom"))
			]
	
			+ SVerticalBox::Slot()
			.Padding(0.0f, 0.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryBottom"))
				.BorderBackgroundColor(FLinearColor(.7f, .7f, .7f))
				[
					SNew(SVerticalBox)
					
					+ SVerticalBox::Slot()
					.Padding(5.0f, 7.0f)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Bottom)
					[
						SAssignNew(EventsBox, SVerticalBox)
					]

					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Bottom)
					.AutoHeight()
					[
						SAssignNew(OutputPinBox, SHorizontalBox) // OUTPUT PIN AREA
					]
				]
			]
		];

	CreateNodeWidget();
	CreatePinWidgets();
}

FReply SGraphNode_DialogNodeBase::OnClickedIcon()
{
	return FReply::Handled();
}

void SGraphNode_DialogNodeBase::CreateNodeWidget()
{
	NodeWiget->SetAutoWrapText(true);
	NodeWiget->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
	NodeWiget->SetText(NodeBace->GetNodeTitle(ENodeTitleType::FullTitle));
}

void SGraphNode_DialogNodeBase::CreatePinWidgets()
{
	for (int i = 0; i < NodeBace->Pins.Num(); i++)
	{
		if (NodeBace->Pins[i]->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			UEdGraphPin* CurPin = NodeBace->Pins[i];
			TSharedPtr<SDialogOutputPin> NewPin = SNew(SDialogOutputPin, CurPin);
			NewPin->SetIsEditable(IsEditable);
			this->AddPin(NewPin.ToSharedRef());
			InputPins.Add(NewPin.ToSharedRef());
		}
		else
		{
			UEdGraphPin* CurPin = NodeBace->Pins[i];
			TSharedPtr<SDialogOutputPin> NewPin = SNew(SDialogOutputPin, CurPin);
			NewPin->SetIsEditable(IsEditable);
			this->AddPin(NewPin.ToSharedRef());
			OutputPins.Add(NewPin.ToSharedRef());
		}
	}
}

void SGraphNode_DialogNodeBase::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		InputPinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(1.0f)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.HeightOverride(25.0f)
				[
					PinToAdd
				]
			];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		OutputPinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillWidth(1.0f)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.HeightOverride(25.0f)
				[
					PinToAdd
				]
			];
		OutputPins.Add(PinToAdd);
	}
}

void SGraphNode_DialogNodeBase::AddTextToContent(TSharedPtr<SVerticalBox> Container, FString Text, FColor Color)
{
	Container->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(Text))
			.ColorAndOpacity(Color)
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGraphNode_DialogNodeBase::OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName)
{
	UpdateGraphNode();
}

FName SGraphNode_DialogNodeBase::GetIcon() const
{
	return "";
}

//RootNode............................................................................................................
SGraphNode_Root::SGraphNode_Root()
{
}

FName SGraphNode_Root::GetIcon() const
{
	return "DialogSystem.Root";
}

//PhraseNode.......................................................................................................
SGraphNode_Phrase::SGraphNode_Phrase()
{
}

void SGraphNode_Phrase::Construct(const FArguments& InArgs, UPhraseNode* InNode)
{
	GraphNode = InNode;
	NodeBace = InNode;
	PhraseNode = InNode;

	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SGraphNode_Phrase::OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName)
{
	UpdateGraphNode();
}

FName SGraphNode_Phrase::GetIcon() const
{
	if (PhraseNode->Data.Source == EDialogPhraseSource::Player)
	{
		return "DialogSystem.Player";
	}
	else
	{
		return "DialogSystem.NPC";
	}
}

void SGraphNode_Phrase::CreateNodeWidget()
{
	SGraphNode_DialogNodeBase::CreateNodeWidget();
	
	for (auto key : PhraseNode->Data.CheckHasKeys)
		AddTextToContent(ConditionsBox, TEXT("HAS KEY ") + key.ToString(), FColor(255, 255, 255));

	for (auto key : PhraseNode->Data.CheckDontHasKeys)
		AddTextToContent(ConditionsBox, TEXT("HAS NOT KEY ") + key.ToString(), FColor(255, 255, 255));

	for (auto key : PhraseNode->Data.CustomConditions)
		AddTextToContent(ConditionsBox, TEXT("IF ") + key.ToString(), FColor(255, 255, 255));
	
	for (auto key : PhraseNode->Data.CustomEvents)
		AddTextToContent(EventsBox, TEXT("E ") + key.ToString(), FColor(255, 0, 0));

	for (auto key : PhraseNode->Data.GiveKeys)
		AddTextToContent(EventsBox, TEXT("+ ") + key.ToString(), FColor(0, 255, 0));

	for (auto key : PhraseNode->Data.RemoveKeys)
		AddTextToContent(EventsBox, TEXT("- ") + key.ToString(), FColor(255, 0, 0));

	ConditionsBox->SetVisibility(ConditionsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
	EventsBox->SetVisibility(EventsBox->NumSlots() > 0 ? EVisibility::Visible : EVisibility::Collapsed);
}

FReply SGraphNode_Phrase::OnClickedIcon()
{
	if (PhraseNode->Data.Source == EDialogPhraseSource::Player)
	{
		PhraseNode->Data.Source = EDialogPhraseSource::NPC;
	}
	else
	{
		PhraseNode->Data.Source = EDialogPhraseSource::Player;
	}

	NodeIcon->SetImage(FBrushSet::Get().GetBrush(GetIcon()));

	return FReply::Handled();
}