#include "DialogSystemEditor.h"
#include "QaDSAssetEditor.h"
#include "Editor.h"
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
#include "HAL/PlatformApplicationMisc.h"
#include "QaDSSettings.h"
#include "QaDSEdGraphNode.h"
#include "DialogGraphSchema.h"

#include "Framework/Application/SlateApplication.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "DesktopPlatformModule.h"
#include "XmlFile.h"

#define LOCTEXT_NAMESPACE "QaDSGraph"

const FName FQaDSAssetEditorTabs::DetailsID(TEXT("Details"));
const FName FQaDSAssetEditorTabs::GraphEditorID(TEXT("Viewport"));
const FName FQaDSAssetEditorTabs::CompilerResultsID(TEXT("CompilerResults"));

TSharedRef<FTabManager::FLayout> FQaDSAssetEditor::GetDefaultLayout()
{
	return FTabManager::NewLayout(*(GetEditorName().ToString() + "Editor_Layout"))
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
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.75f)
						->SetHideTabWell(false)
						->AddTab(FQaDSAssetEditorTabs::DetailsID, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.25f)
						->SetHideTabWell(false)
						->AddTab(FQaDSAssetEditorTabs::CompilerResultsID, ETabState::ClosedTab)
					)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->SetHideTabWell(false)
					->AddTab(FQaDSAssetEditorTabs::GraphEditorID, ETabState::OpenedTab)
				)
			)
		);
}

void FQaDSAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(FText::FromString(GetEditorName().ToString() + " Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FQaDSAssetEditorTabs::GraphEditorID, FOnSpawnTab::CreateSP(this, &FQaDSAssetEditor::SpawnTab_Viewport))
		.SetDisplayName(FText::FromString("Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(FQaDSAssetEditorTabs::CompilerResultsID, FOnSpawnTab::CreateSP(this, &FQaDSAssetEditor::SpawnTab_CompilerResults))
		.SetDisplayName(FText::FromString("CompilerResults"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.BuildAndSubmit"));

	InTabManager->RegisterTabSpawner(FQaDSAssetEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FQaDSAssetEditor::SpawnTab_Details))
		.SetDisplayName(FText::FromString("Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FQaDSAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	
	InTabManager->UnregisterTabSpawner(FQaDSAssetEditorTabs::GraphEditorID);
	InTabManager->UnregisterTabSpawner(FQaDSAssetEditorTabs::CompilerResultsID);
	InTabManager->UnregisterTabSpawner(FQaDSAssetEditorTabs::DetailsID);
}

TSharedRef<SDockTab> FQaDSAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(GetEditorName().ToString() + " Graph"))
		.TabColorScale(GetTabColorScale())
		[
			GraphEditor.ToSharedRef()
		];
}

TSharedRef<SDockTab> FQaDSAssetEditor::SpawnTab_CompilerResults(const FSpawnTabArgs& Args)
{
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	CompilerResultsListing = MessageLogModule.CreateLogListing("CompileResults");
	
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

TSharedRef<SDockTab> FQaDSAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;
	PropertyEditor->OnFinishedChangingProperties().AddRaw(this, &FQaDSAssetEditor::OnPropertyChanged);

	return SNew(SDockTab)
		.Label(FText::FromString("Details"))
		[
			PropertyEditorRef
		];
}

TSharedRef<SGraphEditor> FQaDSAssetEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString(GetEditorName().ToString().ToUpper() + " EDITOR");

	GraphEditorCommands = MakeShareable(new FUICommandList);
	{
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FQaDSAssetEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FQaDSAssetEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FQaDSAssetEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FQaDSAssetEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP(this, &FQaDSAssetEditor::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FQaDSAssetEditor::CanCopyNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP(this, &FQaDSAssetEditor::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FQaDSAssetEditor::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP(this, &FQaDSAssetEditor::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FQaDSAssetEditor::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP(this, &FQaDSAssetEditor::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FQaDSAssetEditor::CanDuplicateNodes)
		);
	}

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FQaDSAssetEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FQaDSAssetEditor::OnNodeDoubleClicked);
	
	TSharedRef<SGraphEditor> _GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);

	return _GraphEditor;
}

void FQaDSAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	TArray<UObject*> SelectedObjects;
	for (UObject* Object : NewSelection)
	{
		SelectedObjects.Add(Object);
	}

	if(SelectedObjects.Num() == 0)
		SelectedObjects.Add(EditedAsset);

	if(PropertyEditor.IsValid())
		PropertyEditor->SetObjects(SelectedObjects);
}

void FQaDSAssetEditor::OpenFindWindow()
{
}

void FQaDSAssetEditor::ExportExecute()
{
	TArray<FString> Filenames;
	bool isSaved = FDesktopPlatformModule::Get()->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(NULL),
		"Choose save location",
		FPaths::ProjectUserDir(),
		TEXT(""),
		TEXT("XML|*.xml"),
		EFileDialogFlags::None,
		Filenames
	);

	if (isSaved)
	{
		FXmlWriteNode nodes("nodes");
		for (auto graphNode : GraphEditor->GetCurrentGraph()->Nodes)
		{
			nodes.Childrens.Add(Cast<UQaDSEdGraphNode>(graphNode)->SaveToXml());
		}

		FFileHelper::SaveStringToFile(nodes.GetXml(), *Filenames[0]);
	}
}

void FQaDSAssetEditor::ImportExecute()
{
	TArray<FString> Filenames;
	bool isOpen = FDesktopPlatformModule::Get()->OpenFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(NULL),
		"Choose file",
		FPaths::ProjectUserDir(),
		TEXT("XML|*.xml"),
		TEXT(""),
		EFileDialogFlags::None,
		Filenames
	);

	if (isOpen)
	{
		FXmlFile xml(*Filenames[0]);

		TMap<UQaDSEdGraphNode*, FXmlReadNode> xmlByNode;
		TMap<FString, UQaDSEdGraphNode*> nodesById;

		for (auto nodeTag : xml.GetRootNode()->GetChildrenNodes())
		{
			auto reader = FXmlReadNode(nodeTag);

			auto nodeClass = FindObject<UClass>(ANY_PACKAGE, *reader.Get("class"));
			if (nodeClass == NULL)
				continue;

			auto nodeTemplate = NewObject<UQaDSEdGraphNode>(GraphEditor->GetCurrentGraph(), nodeClass);
			auto node = FQaDSSchemaAction_NewNode::SpawnNodeFromTemplate<UQaDSEdGraphNode>(EdGraph, nodeTemplate, FVector2D::ZeroVector, false);
			node->AllocateDefaultPins();

			nodesById.Add(reader.Get("id"), node);
			xmlByNode.Add(node, reader);
		}

		if (xmlByNode.Num() == 0)
			return;

		auto graph = GraphEditor->GetCurrentGraph();
		graph->Nodes.Reset();

		for (auto kpv : xmlByNode)
		{
			kpv.Key->LoadInXml(&kpv.Value, nodesById);
			graph->Nodes.Add(kpv.Key);
		}
	}
}

void FQaDSAssetEditor::CompileExecute()
{
	UE_LOG(DialogModuleLog, Log, TEXT("Compile dialog %s"), *EditedAsset->GetPathName());

	CompileLogResults = FCompilerResultsLog();
	CompileLogResults.BeginEvent(TEXT("Compile"));
	CompileLogResults.SetSourcePath(EditedAsset->GetPathName());

	CompileLogResults.Note(TEXT("Compile dialog"));

	if (!CompilerResultsTab.IsValid())
		CompilerResultsTab = TabManager->InvokeTab(FQaDSAssetEditorTabs::CompilerResultsID);

	CompilerResultsListing->ClearMessages();

	Compile();

	CompileLogResults.EndEvent();
	CompilerResultsListing->AddMessages(CompileLogResults.Messages);


	auto selected = GraphEditor->GetSelectedNodes();
	GraphEditor->ClearSelectionSet();

	for (auto n : selected.Array())
		GraphEditor->SetNodeSelection(Cast<UEdGraphNode>(n), true);
}

void FQaDSAssetEditor::ResetCompilePhrase(UQaDSEdGraphNode* Node)
{
	if (!Node->IsCompile())
		return;

	Node->ResetCompile();

	for (auto& child : Node->GetChildNodes())
	{
		auto childPhrase = Cast<UQaDSEdGraphNode>(child);
		if (childPhrase)
			ResetCompilePhrase(childPhrase);
	}
}

void FQaDSAssetEditor::OnNodeDoubleClicked(class UEdGraphNode* Node)
{

}

void FQaDSAssetEditor::SelectAllNodes()
{
	GraphEditor->SelectAllNodes();
}

bool FQaDSAssetEditor::CanSelectAllNodes() const
{
	return GraphEditor.IsValid();
}

void FQaDSAssetEditor::DeleteSelectedNodes()
{
	TArray<UEdGraphNode*> NodesToDelete;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
	}

	DeleteNodes(NodesToDelete);
}

bool FQaDSAssetEditor::CanDeleteNode(class UEdGraphNode* Node)
{
	return true;
}

bool FQaDSAssetEditor::CanDeleteNodes() const
{
	return true;
}

void FQaDSAssetEditor::DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete)
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

void FQaDSAssetEditor::CopySelectedNodes()
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

bool FQaDSAssetEditor::CanCopyNodes() const
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

void FQaDSAssetEditor::PasteNodes()
{
	PasteNodesHere(GraphEditor->GetPasteLocation());
}

void FQaDSAssetEditor::PasteNodesHere(const FVector2D& Location)
{
	const FScopedTransaction Transaction(FText::FromString("Paste"));

	GraphEditor->ClearSelectionSet();

	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	if (EditedAsset == NULL)
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
		
		auto qadesNode = Cast<UQaDSEdGraphNode>(Node);
		if (qadesNode != NULL)
		{
			qadesNode->ResetCompile();
		}
	}

	GraphEditor->NotifyGraphChanged();
}

bool FQaDSAssetEditor::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FQaDSAssetEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FQaDSAssetEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FQaDSAssetEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FQaDSAssetEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FQaDSAssetEditor::DeleteSelectedDuplicatableNodes()
{
	const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

	FGraphPanelSelectionSet RemainingNodes;
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		auto Node = Cast<UEdGraphNode>(*SelectedIter);

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

void FQaDSAssetEditor::OnGraphChanged(const FEdGraphEditAction& Action)
{
	bGraphStateChanged = true;

	if (GetDefault<UQaDSSettings>()->AutoCompile)
		CompileExecute();
}

void FQaDSAssetEditor::OnPropertyChanged(const FPropertyChangedEvent& Event)
{
	if (GetDefault<UQaDSSettings>()->AutoCompile)
		CompileExecute();
}

FName FQaDSAssetEditor::GetToolkitFName() const
{
	return FName(*(GetEditorName().ToString() + " Editor"));
}

FText FQaDSAssetEditor::GetBaseToolkitName() const
{
	return FText::FromString(GetEditorName().ToString() + " EditorToolkit");
}

FString FQaDSAssetEditor::GetWorldCentricTabPrefix() const
{
	return GetEditorName().ToString() + "Editor";
}

FLinearColor FQaDSAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FQaDSAssetEditor::~FQaDSAssetEditor()
{
	if (GraphEditor->GetCurrentGraph())
		GraphEditor->GetCurrentGraph()->RemoveOnGraphChangedHandler(OnGraphChangedDelegateHandle);
}

#undef LOCTEXT_NAMESPACE