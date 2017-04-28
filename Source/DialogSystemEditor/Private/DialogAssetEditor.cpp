// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "DialogAssetEditor.h"
#include "DialogCommands.h"
#include "DialogGraphSchema.h"
#include "SDockTab.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "BlueprintEditorModule.h"
#include "PropertyEditorModule.h"
#include "DialogSettings.h"
#include "DialogEditorNodes.h"

const FName DialogEditorAppName(TEXT("DialogEditorApp"));

const FName FDialogAssetEditorTabs::DetailsID(TEXT("Details"));
const FName FDialogAssetEditorTabs::GraphEditorID(TEXT("Viewport"));
const FName FDialogAssetEditorTabs::CompilerResultsID(TEXT("CompilerResults"));


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

	if (!EditedAsset->UpdateGraph)
	{
		UEdGraph* CustGraph = NewObject<UEdGraph>(EditedAsset, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
		CustGraph->Schema = UDialogGraphSchema::StaticClass();

		FDialogSchemaAction_NewNode::SpawnNodeFromTemplate<URootNode>(CustGraph, NewObject<URootNode>(), FVector2D::ZeroVector, false);

		EditedAsset->UpdateGraph = CustGraph;
	}

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
					->SetHideTabWell(true)
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
	ToolkitCommands->MapAction(FDialogCommands::Get().Compile, FExecuteAction::CreateSP(this, &FDialogAssetEditor::CompileExecute));

	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FDialogAssetEditor::BuildToolbar));
	AddToolbarExtender(ToolbarExtender);

	InitAssetEditor(Mode, InitToolkitHost, DialogEditorAppName, StandaloneDefaultLayout, /*bCreateDefaultStandaloneMenu=*/ true, /*bCreateDefaultToolbar=*/ true, Object);

	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FDialogAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FDialogAssetEditor::BuildToolbar(FToolBarBuilder &builder)
{
	FSlateIcon IconBrushCompile = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");

	builder.AddToolBarButton(FDialogCommands::Get().Compile, NAME_None, FText::FromString("Compile"), FText::FromString("Compile dialog graph"), IconBrushCompile, NAME_None);
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


// Editor Actions......................................................................................................

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
			if (!CanDeleteNode(NodesToDelete[Index])) {
				continue;
			}

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
	FPlatformMisc::ClipboardCopy(*ExportedText);
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
	FPlatformMisc::ClipboardPaste(TextToImport);

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
			phraseNode->UID.Invalidate();
		}
	}

	GraphEditor->NotifyGraphChanged();
}

bool FDialogAssetEditor::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformMisc::ClipboardPaste(ClipboardContent);

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
	if (GetDefault<UDialogSettings>()->AutoCompile)
		CompileExecute();
}

FDialogAssetEditor::~FDialogAssetEditor()
{
	if (GraphEditor->GetCurrentGraph())
	{
		GraphEditor->GetCurrentGraph()->RemoveOnGraphChangedHandler(OnGraphChangedDelegateHandle);
		//OnPropertyChangedDelegateHandle
	}
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

	UDialogPhrase* rootPhrase = NULL;
	for (auto node : GraphEditor->GetCurrentGraph()->Nodes)
	{
		auto root = Cast<URootNode>(node);
		if (root)
		{
			if (rootPhrase != NULL)
			{
				CompileLogResults.Error(TEXT("Multiple root nodes found"));
				break;
			}
			ResetCompilePhrase(root);
			rootPhrase = Compile(root);
		}
	}

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

	phrase->OwnerDialog = Cast<UDialogAsset>(EditedAsset);
	phrase->Text = Node->Text;
	phrase->AutoTime = Node->AutoTime;
	phrase->Important = Node->Important;
	phrase->PhraseManualTime = Node->PhraseManualTime;
	phrase->UID = Node->UID;
	phrase->Sound = Node->Sound;
	phrase->IsPlayer = Node->IsPlayer;
	phrase->CheckHasKeys = Node->CheckHasKeys;
	phrase->CheckDontHasKeys = Node->CheckDontHasKeys;
	phrase->GiveKeys = Node->GiveKeys;
	phrase->RemoveKeys = Node->RemoveKeys;

	if (Node->MemorizeReading)
	{
		if (!Node->UID.IsValid())
			Node->UID = FGuid::NewGuid();
	}
	else
		Node->UID.Invalidate();

	phrase->UID = Node->UID;

	for (auto& Event : Node->CustomEvents)
	{
		FString ErrorMessage;

		Event.OwnerNode = phrase;
		if (Event.Compile(ErrorMessage))
			phrase->AddEvent(&phrase->CustomEvents, Event);
		else
			CompileLogResults.Error(*ErrorMessage);
	}

	for (auto& Condition : Node->CustomConditions)
	{
		FString ErrorMessage;

		Condition.OwnerNode = phrase;
		if (Condition.Compile(ErrorMessage))
			phrase->AddCondition(&phrase->CustomConditions, Condition);
		else
			CompileLogResults.Error(*ErrorMessage);
	}

	bool first = true;
	bool isPlayer = false;

	for (auto& child : Node->GetChildNodes())
	{
		auto childPhrase = Cast<UPhraseNode>(child);

		if (childPhrase)
		{
			if (first)
			{
				isPlayer = childPhrase->IsPlayer;
				first = false;
			}
			else if (isPlayer != childPhrase->IsPlayer)
			{
				CompileLogResults.Error(TEXT("Invalid graph: Phrase cannot simultaneously refer to phrases of different types"));
			}

			phrase->Childs.Add(Compile(childPhrase));
		}
	}
	return phrase;
}