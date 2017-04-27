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

	typedef SDialogOutputPin ThisClass;

	bShowLabel = true;
	IsEditable = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SDialogOutputPin::GetPinBorder)
		.BorderBackgroundColor(this, &ThisClass::GetPinColor)
		.OnMouseButtonDown(this, &ThisClass::OnPinMouseDown)
		.Cursor(this, &ThisClass::GetPinCursor)
		.Padding(FMargin(5.0f))
	);
}

TSharedRef<SWidget>	SDialogOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SDialogOutputPin::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Body"));
}

FSlateColor SDialogOutputPin::GetPinColor() const
{
	SGraphNode_Phrase* phraseNode = NULL;

	if (OwnerNodePtr.IsValid())
		phraseNode = StaticCast<SGraphNode_Phrase*>(OwnerNodePtr.Pin().Get());

	FLinearColor defaultColor;

	if (phraseNode && phraseNode->GetNodeObj())
		defaultColor = Cast<UPhraseNode>(phraseNode->GetNodeObj())->GetNodeTitleColor();
	else
		GetDefault<UDialogSettings>()->NodeButtonDefault;

	return IsHovered() ? GetDefault<UDialogSettings>()->NodeButtonHovered : defaultColor;
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
	IconBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
			.Padding(2)
			.BorderBackgroundColor(this, &SGraphNode_Root::GetBorderBackgroundColor)
			[
				SNew(SVerticalBox)

				// INPUT PIN AREA
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.AutoHeight()
				[
					SAssignNew(InputPinBox, SHorizontalBox)
				]

				// CONTENT AREA
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillHeight(1.0f)
				[
					SNew(SHorizontalBox)

					// ICONS AREA
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					.Padding(5.0f, 10.0f, 5.0f, 0.0f)
					.AutoWidth()
					[
						SAssignNew(IconBox, SVerticalBox)
					]

					// TEXT AREA
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.FillWidth(1.0f)
					.Padding(10.0f, 5.0f, 10.0f, 5.0f)
					[
						SAssignNew(ContentBox, SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						[
							SNew(SBox)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							.MinDesiredHeight(70.0f)
							.WidthOverride(320.0f)
							.MaxDesiredWidth(320.0f)
							[
								SAssignNew(NodeWiget, STextBlock)
							]
						]
					]
				]

				// OUTPUT PIN AREA
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Bottom)
				.AutoHeight()
				[
					SAssignNew(OutputPinBox, SHorizontalBox)
				]
			]
		];

	CreateNodeWidget();
	CreatePinWidgets();
}

void SGraphNode_DialogNodeBase::CreateNodeWidget()
{
	NodeWiget->SetAutoWrapText(true);
	NodeWiget->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
	NodeWiget->SetText(NodeBace->GetNodeTitle(ENodeTitleType::FullTitle));

	NodeWiget->SetFont(FSlateFontInfo(
		FPaths::GamePluginsDir() / TEXT("DialogSystem/Resources/MainFont.ttf"), 
		GetDefault<UDialogSettings>()->FontSize));
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

void SGraphNode_DialogNodeBase::AddIconIF(FName BrushName, bool condition)
{
	if (condition)
	{
		IconBox->AddSlot()
			.Padding(0, 0.0f, 0.0f, 5.0f)
			[
				SNew(SImage)
				.Image(FBrushSet::Get().GetBrush(BrushName))
			];
	}
}

void SGraphNode_DialogNodeBase::AddTextToContent(FString Text, FColor Color)
{
	ContentBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(Text))
			.ColorAndOpacity(Color)
			.Font(FSlateFontInfo(FPaths::GamePluginsDir() / TEXT("DialogSystem/Resources/MainFont.ttf"), GetDefault<UDialogSettings>()->FontSize))
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGraphNode_DialogNodeBase::OnPropertyChanged(UEdGraphNode* Sender, const FName& PropertyName)
{
	UpdateGraphNode();
}

FSlateColor SGraphNode_DialogNodeBase::GetBorderBackgroundColor() const
{
	return BackgroundColor;
}


//RootNode............................................................................................................
SGraphNode_Root::SGraphNode_Root()
{
	BackgroundColor = GetDefault<UDialogSettings>()->NodeRoot;
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

FSlateColor SGraphNode_Phrase::GetBorderBackgroundColor() const
{
	return PhraseNode != NULL ? PhraseNode->GetNodeTitleColor() :
		SGraphNode_DialogNodeBase::GetBorderBackgroundColor();
}

void SGraphNode_Phrase::CreateNodeWidget()
{
	SGraphNode_DialogNodeBase::CreateNodeWidget();
	
	AddIconIF("DialogSystem.Sound", PhraseNode->Sound != NULL);
	AddIconIF("DialogSystem.Time", !PhraseNode->AutoTime);
	AddIconIF("DialogSystem.Book", PhraseNode->Important);
	AddIconIF("DialogSystem.Event", PhraseNode->CustomEvents.Num() != 0);
	AddIconIF("DialogSystem.Condition", PhraseNode->CustomConditions.Num() != 0);

	for (auto key : PhraseNode->CheckHasKeys)
		AddTextToContent(TEXT("IF ") + key.ToString(), FColor(255, 255, 255));

	for (auto key : PhraseNode->CheckDontHasKeys)
		AddTextToContent(TEXT("IF NOT ") + key.ToString(), FColor(255, 255, 255));

	for (auto key : PhraseNode->GiveKeys)
		AddTextToContent(TEXT("+ ") + key.ToString(), FColor(0, 255, 0));

	for (auto key : PhraseNode->RemoveKeys)
		AddTextToContent(TEXT("- ") + key.ToString(), FColor(255, 0, 0));
}