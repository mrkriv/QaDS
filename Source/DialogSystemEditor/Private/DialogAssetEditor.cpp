// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "DialogAssetEditor.h"
#include "Editor.h"
#include "DialogGraphSchema.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "Kismet2/DebuggerCommands.h"
#include "ScopedTransaction.h"
#include "SDockTab.h"
#include "SNodePanel.h"
#include "SGraphPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/SListView.h"
#include "BlueprintEditorModule.h"
#include "PropertyEditorModule.h"
#include "DialogSettings.h"
#include "DialogEditorNodes.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "DialogGraph"

const FName DialogEditorAppName(TEXT("DialogEditorApp"));

const FName FDialogAssetEditorTabs::DetailsID(TEXT("Details"));
const FName FDialogAssetEditorTabs::GraphEditorID(TEXT("Viewport"));
const FName FDialogAssetEditorTabs::CompilerResultsID(TEXT("CompilerResults"));

void FDialogCommands::RegisterCommands()
{
	UI_COMMAND(Compile, "Compile", "Compile this dialog graph", EUserInterfaceActionType::Button, FInputChord());
}

FName FDialogAssetEditor::GetToolkitFName() const
{
	return FName("Dialog Editor");
}

FText FDialogAssetEditor::GetBaseToolkitName() const
{
	return FText::FromString("Dialog EditorToolkitName");
}

FLinearColor FDialogAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FDialogAssetEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("DialogEditor");
}

void FDialogAssetEditor::InitDialogAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UDialogAsset* Object)
{
	FAssetEditorManager::Get().CloseOtherEditors(Object, this);
	EditedAsset = Object;

	if (EditedAsset->UpdateGraph == NULL)
		EditedAsset->UpdateGraph = CreateUpdateGraph();
	
	GraphEditor = CreateGraphEditorWidget(EditedAsset->UpdateGraph);

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("DialogEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.8f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(false)
					->AddTab(FDialogAssetEditorTabs::GraphEditorID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(false)
					->AddTab(FDialogAssetEditorTabs::DetailsID, ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(FDialogAssetEditorTabs::CompilerResultsID, ETabState::ClosedTab)
			)
		);

	ToolkitCommands = MakeShareable(new FUICommandList);
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
	ToolkitCommands->MapAction(FDialogCommands::Get().Compile, FExecuteAction::CreateSP(this, &FDialogAssetEditor::CompileExecute));
	
	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FDialogAssetEditor::BuildToolbar));
	AddToolbarExtender(ToolbarExtender);

	InitAssetEditor(Mode, InitToolkitHost, DialogEditorAppName, StandaloneDefaultLayout, true, true, Object);

	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FDialogAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FDialogAssetEditor::BuildToolbar(FToolBarBuilder &builder)
{
	FSlateIcon IconBrushCompile = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");
	FSlateIcon IconBrushKeys = FSlateIcon(FEditorStyle::GetStyleSetName(), "FullBlueprintEditor.EditClassDefaults", "FullBlueprintEditor.EditClassDefaults.Small");
	FSlateIcon IconBrushKeyManager = FSlateIcon(FEditorStyle::GetStyleSetName(), "FullBlueprintEditor.EditClassDefaults", "FullBlueprintEditor.EditClassDefaults.Small");

	builder.AddSeparator();
	builder.AddToolBarButton(FDialogCommands::Get().Compile, NAME_None, FText::FromString("Compile"), FText::FromString("Compile this dialog"), IconBrushCompile, NAME_None);
	builder.AddSeparator();

	FPlayWorldCommands::BuildToolbar(builder);
}

void FDialogAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(FText::FromString("Dialog Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FDialogAssetEditorTabs::GraphEditorID, FOnSpawnTab::CreateSP(this, &FDialogAssetEditor::SpawnTab_Viewport))
		.SetDisplayName(FText::FromString("Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FDialogAssetEditorTabs::CompilerResultsID, FOnSpawnTab::CreateSP(this, &FDialogAssetEditor::SpawnTab_CompilerResults))
		.SetDisplayName(FText::FromString("CompilerResults"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.BuildAndSubmit"));

	InTabManager->RegisterTabSpawner(FDialogAssetEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FDialogAssetEditor::SpawnTab_Details))
		.SetDisplayName(FText::FromString("Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FDialogAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	
	InTabManager->UnregisterTabSpawner(FDialogAssetEditorTabs::GraphEditorID);
	InTabManager->UnregisterTabSpawner(FDialogAssetEditorTabs::CompilerResultsID);
	InTabManager->UnregisterTabSpawner(FDialogAssetEditorTabs::DetailsID);
}

TSharedRef<SDockTab> FDialogAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString("Dialog Graph"))
		.TabColorScale(GetTabColorScale())
		[
			GraphEditor.ToSharedRef()
		];
}

TSharedRef<SDockTab> FDialogAssetEditor::SpawnTab_CompilerResults(const FSpawnTabArgs& Args)
{
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	CompilerResultsListing = MessageLogModule.CreateLogListing("DialogCompileResults");
	
	return SNew(SDockTab)
		.Label(FText::FromString("Compile Results"))
		.TabColorScale(GetTabColorScale())
		.OnTabClosed_Lambda([this](auto _)
		{
			CompilerResultsTab.Reset();
		})
		[
			MessageLogModule.CreateLogListingWidget(CompilerResultsListing.ToSharedRef())
		];
}

TSharedRef<SDockTab> FDialogAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;
	PropertyEditor->OnFinishedChangingProperties().AddRaw(this, &FDialogAssetEditor::OnPropertyChanged);

	return SNew(SDockTab)
		.Label(FText::FromString("Details"))
		[
			PropertyEditorRef
		];
}

TSharedRef<SGraphEditor> FDialogAssetEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString("DIALOG EDITOR");

	GraphEditorCommands = MakeShareable(new FUICommandList);
	{
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FDialogAssetEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FDialogAssetEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FDialogAssetEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FDialogAssetEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP(this, &FDialogAssetEditor::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FDialogAssetEditor::CanCopyNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP(this, &FDialogAssetEditor::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FDialogAssetEditor::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP(this, &FDialogAssetEditor::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FDialogAssetEditor::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP(this, &FDialogAssetEditor::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FDialogAssetEditor::CanDuplicateNodes)
		);
	}

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FDialogAssetEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FDialogAssetEditor::OnNodeDoubleClicked);
	
	TSharedRef<SGraphEditor> _GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);

	return _GraphEditor;
}

void FDialogAssetEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> SelectedObjects;
	for (UObject* Object : NewSelection)
	{
		if (!Cast<URootNode>(Object))
			SelectedObjects.Add(Object);
	}

	if(SelectedObjects.Num() == 0)
		SelectedObjects.Add(EditedAsset);

	if(PropertyEditor.IsValid())
		PropertyEditor->SetObjects(SelectedObjects);
}

void FDialogAssetEditor::OnNodeDoubleClicked(class UEdGraphNode* Node)
{

}

void FDialogAssetEditor::SelectAllNodes()
{
	GraphEditor->SelectAllNodes();
}

bool FDialogAssetEditor::CanSelectAllNodes() const
{
	return GraphEditor.IsValid();
}

void FDialogAssetEditor::DeleteSelectedNodes()
{
	TArray<UEdGraphNode*> NodesToDelete;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
	}

	DeleteNodes(NodesToDelete);
}

bool FDialogAssetEditor::CanDeleteNode(class UEdGraphNode* Node)
{
	return true;
}

bool FDialogAssetEditor::CanDeleteNodes() const
{
	return true;
}

void FDialogAssetEditor::DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete)
{
	if (NodesToDelete.Num() > 0)
	{
		for (int32 Index = 0; Index < NodesToDelete.Num(); ++Index)
		{
			if (!CanDeleteNode(NodesToDelete[Index]))
				continue;

			NodesToDelete[Index]->BreakAllNodeLinks();

			FBlueprintEditorUtils::RemoveNode(NULL, NodesToDelete[Index], true);
		}
	}
}

void FDialogAssetEditor::CopySelectedNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
			Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FDialogAssetEditor::CanCopyNodes() const
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
			return true;
	}
	return false;
}

void FDialogAssetEditor::PasteNodes()
{
	PasteNodesHere(GraphEditor->GetPasteLocation());
}

void FDialogAssetEditor::PasteNodesHere(const FVector2D& Location)
{
	const FScopedTransaction Transaction(FText::FromString("Paste"));

	GraphEditor->ClearSelectionSet();

	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	if (!EditedAsset)
		return;

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport, PastedNodes);

	FVector2D AvgNodePosition(0.0f, 0.0f);

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	if (PastedNodes.Num() > 0)
	{
		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;

		GraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		Node->CreateNewGuid();
		
		auto phraseNode = Cast<UPhraseNode>(Node);
		if (phraseNode != NULL)
		{
			phraseNode->CompilePhrase = NULL;
		}
	}

	GraphEditor->NotifyGraphChanged();
}

bool FDialogAssetEditor::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FDialogAssetEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FDialogAssetEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FDialogAssetEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FDialogAssetEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FDialogAssetEditor::DeleteSelectedDuplicatableNodes()
{
	const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

	FGraphPanelSelectionSet RemainingNodes;
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);

		if ((Node != NULL) && Node->CanDuplicateNode())
			GraphEditor->SetNodeSelection(Node, true);
		else
			RemainingNodes.Add(Node);
	}

	DeleteSelectedNodes();

	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			GraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FDialogAssetEditor::OnGraphChanged(const FEdGraphEditAction& Action)
{
	bGraphStateChanged = true;

	if (GetDefault<UDialogSettings>()->AutoCompile)
		CompileExecute();
}

void FDialogAssetEditor::OnPropertyChanged(const FPropertyChangedEvent& Event)
{
	if (Event.MemberProperty->GetFName().ToString() == TEXT("CustomEvents") &&
		Event.Property->GetFName().ToString() == TEXT("EventName") ||
		Event.MemberProperty->GetFName().ToString() == TEXT("CustomCondition") &&
		Event.Property->GetFName().ToString() == TEXT("EventName"))
	{
		auto selected = GraphEditor->GetSelectedNodes();
		GraphEditor->ClearSelectionSet();

		for (auto n : selected.Array())
			GraphEditor->SetNodeSelection(Cast<UEdGraphNode>(n), true);
	}

	if (GetDefault<UDialogSettings>()->AutoCompile)
		CompileExecute();
}

URootNode* FDialogAssetEditor::GetRootNode()
{
	for (auto node : GraphEditor->GetCurrentGraph()->Nodes)
	{
		if (URootNode* root = Cast<URootNode>(node))
			return root;
	}

	return NULL;
}

UEdGraph* FDialogAssetEditor::CreateUpdateGraph()
{
	auto CustGraph = NewObject<UEdGraph>(EditedAsset, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
	CustGraph->Schema = UDialogGraphSchema::StaticClass();

	auto root = FDialogSchemaAction_NewNode::SpawnNodeFromTemplate<URootNode>(CustGraph, NewObject<URootNode>(), FVector2D::ZeroVector, false);
	root->AllocateDefaultPins();

	for (auto phrase : EditedAsset->RootNode->Childs)
		CreateNodesFromPhrase(root, phrase, 1);

	return CustGraph;
}

void FDialogAssetEditor::CreateNodesFromPhrase(UPhraseNode* owner, UDialogNode* phrase, int level)
{
	auto dialogPhrase = Cast<UDialogPhrase>(phrase);

	if (dialogPhrase == NULL)
		return;

	auto location = FVector2D(owner->OutputPin->LinkedTo.Num() * 400, level * 250);

	auto node = FDialogSchemaAction_NewNode::SpawnNodeFromTemplate<UPhraseNode>(owner->GetGraph(), NewObject<UPhraseNode>(), location, false);
	node->CompilePhrase = dialogPhrase;
	node->Data = dialogPhrase->Data;

	node->InputPin->MakeLinkTo(owner->OutputPin);

	for (auto child : phrase->Childs)
		CreateNodesFromPhrase(node, child, level + 1);
}

void FDialogAssetEditor::CompileExecute()
{
	UE_LOG(DialogModuleLog, Log, TEXT("Compile dialog %s"), *EditedAsset->GetPathName());

	CompileLogResults = FCompilerResultsLog();
	CompileLogResults.BeginEvent(TEXT("Compile"));
	CompileLogResults.SetSourcePath(EditedAsset->GetPathName());

	CompileLogResults.Note(TEXT("Compile dialog"));

	if (!CompilerResultsTab.IsValid())
		CompilerResultsTab = TabManager->InvokeTab(FDialogAssetEditorTabs::CompilerResultsID);

	CompilerResultsListing->ClearMessages();

	auto rootNode = GetRootNode();
	ResetCompilePhrase(rootNode);

	auto rootPhrase = Compile(rootNode);

	if (rootPhrase != NULL)
	{
		EditedAsset->RootNode = rootPhrase;
	}
	else
		CompileLogResults.Error(TEXT("Root node not found"));

	CompileLogResults.EndEvent();
	CompilerResultsListing->AddMessages(CompileLogResults.Messages);
}

void FDialogAssetEditor::ResetCompilePhrase(UPhraseNode* Node)
{
	if (Node->CompilePhrase == NULL)
		return;

	Node->CompilePhrase = NULL;

	for (auto& child : Node->GetChildNodes())
	{
		auto childPhrase = Cast<UPhraseNode>(child);
		if (childPhrase)
			ResetCompilePhrase(childPhrase);
	}
}

UDialogPhrase* FDialogAssetEditor::Compile(UPhraseNode* Node)
{
	if (Node->CompilePhrase != NULL)
		return Node->CompilePhrase;

	auto phrase = NewObject<UDialogPhrase>((UObject*)EditedAsset);
	Node->CompilePhrase = phrase;
	Node->Data.UID = *Node->NodeGuid.ToString();

	phrase->OwnerDialog = Cast<UDialogAsset>(EditedAsset);
	phrase->Data = Node->Data;

	bool needUpdate = false;

	FString ErrorMessage;

	for (auto& Event : Node->Data.CustomEvents)
	{
		Event.OwnerNode = phrase;

		if (!Event.Compile(ErrorMessage, needUpdate))
		{
			CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + Node->Data.Text.ToString() + "\""));
		}
	}

	for (auto& Condition : Node->Data.CustomConditions)
	{
		Condition.OwnerNode = phrase;

		if (!Condition.Compile(ErrorMessage, needUpdate))
		{
			CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + Node->Data.Text.ToString() + "\""));
		}
	}

	auto childs = Node->GetChildNodes();
	childs.Sort([](auto& a, auto& b)
	{
		return a.NodePosX < b.NodePosX;
	});

	bool first = true;
	EDialogPhraseSource source = EDialogPhraseSource::NPC;

	for (auto& child : childs)
	{
		auto childPhrase = Cast<UPhraseNode>(child);

		if (childPhrase)
		{
			if (first)
			{
				source = childPhrase->Data.Source;
				first = false;
			}
			else if (source != childPhrase->Data.Source)
			{
				CompileLogResults.Error(TEXT("Invalid graph: Phrase cannot simultaneously refer to phrases of different types"));
			}

			phrase->Childs.Add(Compile(childPhrase));
		}
	}

	if (needUpdate && PropertyEditor.IsValid())
	{
		for (auto& obj : PropertyEditor->GetSelectedObjects())
		{
			if (obj == Node)
			{
				auto selected = GraphEditor->GetSelectedNodes();
				GraphEditor->ClearSelectionSet();

				for (auto n : selected.Array())
					GraphEditor->SetNodeSelection(Cast<UEdGraphNode>(n), true);

				break;
			}
		}
	}

	return phrase;
}

FDialogAssetEditor::~FDialogAssetEditor()
{
	if (GraphEditor->GetCurrentGraph())
		GraphEditor->GetCurrentGraph()->RemoveOnGraphChangedHandler(OnGraphChangedDelegateHandle);
}

#undef LOCTEXT_NAMESPACE