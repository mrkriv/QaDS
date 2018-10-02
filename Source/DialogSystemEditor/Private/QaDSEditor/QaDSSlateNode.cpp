#include "DialogSystemEditor.h"
#include "QaDSSlateNode.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "RectConnectionDrawingPolicy.h"
#include "DialogGraphSchema.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "FQaDSSystemModule"

//PinNode..........................................................................
void SQaDSOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;
	IsEditable = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SQaDSOutputPin::GetPinBorder)
		.BorderBackgroundColor(this, &SQaDSOutputPin::GetPinColor)
		.OnMouseButtonDown(this, &SQaDSOutputPin::OnPinMouseDown)
		.Cursor(this, &SQaDSOutputPin::GetPinCursor)
		.Padding(FMargin(5.0f))
	);
}

TSharedRef<SWidget>	SQaDSOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SQaDSOutputPin::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("DetailsView.CategoryMiddle"));
}

FSlateColor SQaDSOutputPin::GetPinColor() const
{
	FLinearColor defaultColor = FLinearColor(0.9f, 0.9f, 0.9f);

	return IsHovered() ? FLinearColor(0.65f, 0.65f, 0.65f) : defaultColor;
}

//DialogNodeBase............................................................................................................

void SGraphNode_QaDSNodeBase::Construct(const FArguments& InArgs, UQaDSEdGraphNode* InNode)
{
	SetCursor(EMouseCursor::CardinalCross);
	GraphNode = InNode;
	UpdateGraphNode();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SGraphNode_QaDSNodeBase::UpdateGraphNode()
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
						SAssignNew(InputPinBox, SHorizontalBox)
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
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
			]
	
			+ SVerticalBox::Slot()
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
					.Padding(-7.0f, 0.0f, -5.0f, 0.0f)
					.AutoWidth()
					[
						SNew(SButton)
						.OnClicked(this, &SGraphNode_QaDSNodeBase::OnClickedIcon)
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
						SAssignNew(BodyBox, SVerticalBox)

						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.FillHeight(1.0f)
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
					

					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Top)
					.Padding(10.0f, -5.0f, 0.0f, 0.0f)
					.AutoWidth()
					[
						SAssignNew(OrderDisplayBorder, SBorder)
						[
							SAssignNew(OrderDisplayText, STextBlock)
						]
					]
				]
			]

			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryBottom"))
			]
	
			+ SVerticalBox::Slot()
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
						SAssignNew(OutputPinBox, SHorizontalBox)
					]
				]
			]
		];

	CreateNodeWidget();
	CreatePinWidgets();
}

FReply SGraphNode_QaDSNodeBase::OnClickedIcon()
{
	return FReply::Handled();
}

void SGraphNode_QaDSNodeBase::CreateNodeWidget()
{
	NodeWiget->SetAutoWrapText(true);
	NodeWiget->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
	NodeWiget->SetText(GraphNode->GetNodeTitle(ENodeTitleType::FullTitle));
}

void SGraphNode_QaDSNodeBase::CreatePinWidgets()
{
	for (auto pin : GraphNode->Pins)
	{
		auto NewPin = SNew(SQaDSOutputPin, pin);
		NewPin->SetIsEditable(IsEditable);
		AddPin(NewPin);

		if (pin->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			InputPins.Add(NewPin);
		}
		else
		{
			OutputPins.Add(NewPin);
		}
	}
}

void SGraphNode_QaDSNodeBase::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	auto PinObj = PinToAdd->GetPinObj();
	auto bAdvancedParameter = PinObj && PinObj->bAdvancedView;
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

void SGraphNode_QaDSNodeBase::AddTextToContent(TSharedPtr<SVerticalBox> Container, const FString& Prefix, const FString& Text, const FColor& Color)
{
	auto prefixColor = Color;
	prefixColor.A *= 0.7f;

	Container->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Prefix))
				.ColorAndOpacity(prefixColor)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(5.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Text))
				.ColorAndOpacity(Color)
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGraphNode_QaDSNodeBase::OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName)
{
	UpdateGraphNode();
}

void SGraphNode_QaDSNodeBase::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime)
{
	SGraphNode::Tick(AllottedGeometry, InCurrentTime, DeltaTime);

	auto order = CastChecked<UQaDSEdGraphNode>(GraphNode)->GetOrder();

	OrderDisplayBorder->SetVisibility(order > 0 ? EVisibility::Visible : EVisibility::Hidden);
	OrderDisplayText->SetText(FText::AsNumber(order));
}

#undef LOCTEXT_NAMESPACE