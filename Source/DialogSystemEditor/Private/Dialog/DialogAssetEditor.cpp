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
#include "QaDSSettings.h"
#include "DialogNodes.h"
#include "DialogEditorNodes.h"
#include "HAL/PlatformApplicationMisc.h"

#include "Framework/Application/SlateApplication.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "DesktopPlatformModule.h"
#include "XmlFile.h"

#define LOCTEXT_NAMESPACE "DialogGraph"

const FName DialogEditorAppName(TEXT("DialogEditorApp"));

void FDialogCommands::RegisterCommands()
{
	UI_COMMAND(Compile, "Compile", "Compile this dialog graph", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Find, "Find", "Open find dialog", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Import, "Import", "Imoprt graph from file", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Export, "Export", "Export graph to file", EUserInterfaceActionType::Button, FInputChord());
}

FName FDialogAssetEditor::GetEditorName() const
{
	return TEXT("Dialog"); 
}

UObject* FDialogAssetEditor::GetEditedAsset() const
{
	return EditedAsset;
}

void FDialogAssetEditor::InitDialogAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UDialogAsset* Object)
{
	FAssetEditorManager::Get().CloseOtherEditors(Object, this);
	EditedAsset = Object;

	if (EditedAsset->UpdateGraph == NULL)
		EditedAsset->UpdateGraph = CreateGraphFromAsset();
	
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
					->AddTab(FQaDSAssetEditorTabs::GraphEditorID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(false)
					->AddTab(FQaDSAssetEditorTabs::DetailsID, ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(FQaDSAssetEditorTabs::CompilerResultsID, ETabState::ClosedTab)
			)
		);

	ToolkitCommands = MakeShareable(new FUICommandList);
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
	ToolkitCommands->MapAction(FDialogCommands::Get().Compile, FExecuteAction::CreateSP(this, &FDialogAssetEditor::CompileExecute));
	ToolkitCommands->MapAction(FDialogCommands::Get().Find, FExecuteAction::CreateSP(this, &FDialogAssetEditor::OpenFindWindow));
	ToolkitCommands->MapAction(FDialogCommands::Get().Import, FExecuteAction::CreateSP(this, &FDialogAssetEditor::ImportExecute));
	ToolkitCommands->MapAction(FDialogCommands::Get().Export, FExecuteAction::CreateSP(this, &FDialogAssetEditor::ExportExecute));
	
	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FDialogAssetEditor::BuildToolbar));
	AddToolbarExtender(ToolbarExtender);

	InitAssetEditor(Mode, InitToolkitHost, DialogEditorAppName, StandaloneDefaultLayout, true, true, Object);

	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FDialogAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FDialogAssetEditor::BuildToolbar(FToolBarBuilder &builder)
{
	auto iconCompile = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");
	auto iconFind = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");
	auto iconExport = FSlateIcon(FEditorStyle::GetStyleSetName(), "");
	auto iconImport = FSlateIcon(FEditorStyle::GetStyleSetName(), "");

	builder.AddSeparator();
	builder.AddToolBarButton(FDialogCommands::Get().Compile, NAME_None, FText::FromString("Compile"), FText::FromString("Compile this dialog"), iconCompile, NAME_None);
	builder.AddSeparator(); 
	builder.AddToolBarButton(FDialogCommands::Get().Find, NAME_None, FText::FromString("Find"), FText::FromString("Open find dialog"), iconFind, NAME_None);
	builder.AddSeparator(); 
	builder.AddToolBarButton(FDialogCommands::Get().Export, NAME_None, FText::FromString("Export"), FText::FromString("Export graph to file"), iconImport, NAME_None);
	builder.AddToolBarButton(FDialogCommands::Get().Import, NAME_None, FText::FromString("Import"), FText::FromString("Imoprt graph from file"), iconExport, NAME_None);
	builder.AddSeparator();

	FPlayWorldCommands::BuildToolbar(builder);
}

FString FDialogAssetEditor::ExportToXml()
{
	FString xml = "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n";
	xml += "<nodes>\n";

	for (auto graphNode : GraphEditor->GetCurrentGraph()->Nodes)
	{
		xml += "\t<node>\n";
		xml += Cast<UDialogEdGraphNode>(graphNode)->SaveToXml(2);
		xml += "\t</node>\n";
	}
	xml += "</nodes>";

	return xml;
}

void FDialogAssetEditor::ImportFromXml(FXmlFile* xml)
{
	TMap<UDialogEdGraphNode*, FXmlNode*> xmlByNode;
	TMap<FString, UDialogEdGraphNode*> nodesById;

	for (auto nodeTag : xml->GetRootNode()->GetChildrenNodes())
	{
		auto idTag = nodeTag->FindChildNode("id");
		auto classTag = nodeTag->FindChildNode("class");

		if (idTag == NULL || classTag == NULL)
			continue;

		auto nodeClass = FindObject<UClass>(ANY_PACKAGE, *classTag->GetContent());

		auto nodeTemplate = NewObject<UDialogEdGraphNode>(EditedAsset, nodeClass);
		auto node = FDialogSchemaAction_NewNode::SpawnNodeFromTemplate<UDialogEdGraphNode>(EditedAsset->UpdateGraph, nodeTemplate, FVector2D::ZeroVector, false);
		node->AllocateDefaultPins();

		FGuid::Parse(idTag->GetContent(), node->NodeGuid);

		nodesById.Add(idTag->GetContent(), node);
		xmlByNode.Add(node, nodeTag);
	}

	if (xmlByNode.Num() == 0)
		return;

	auto graph = GraphEditor->GetCurrentGraph();
	graph->Nodes.Reset();

	for (auto kpv : xmlByNode)
	{
		kpv.Key->LoadInXml(kpv.Value, nodesById);
		graph->Nodes.Add(kpv.Key);
	}
}

UDialogRootEdGraphNode* FDialogAssetEditor::GetRootNode()
{
	for (auto node : GraphEditor->GetCurrentGraph()->Nodes)
	{
		if (UDialogRootEdGraphNode* root = Cast<UDialogRootEdGraphNode>(node))
			return root;
	}

	return NULL;
}

UEdGraph* FDialogAssetEditor::CreateGraphFromAsset()
{
	auto CustGraph = NewObject<UEdGraph>(EditedAsset, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
	CustGraph->Schema = UDialogGraphSchema::StaticClass();

	auto root = FDialogSchemaAction_NewNode::SpawnNodeFromTemplate<UDialogRootEdGraphNode>(CustGraph, NewObject<UDialogRootEdGraphNode>(), FVector2D::ZeroVector, false);
	root->AllocateDefaultPins();

	return CustGraph;
}

void FDialogAssetEditor::CompileExecute()
{
	UE_LOG(DialogModuleLog, Log, TEXT("Compile dialog %s"), *EditedAsset->GetPathName());

	CompileLogResults = FCompilerResultsLog();
	CompileLogResults.BeginEvent(TEXT("Compile"));
	CompileLogResults.SetSourcePath(EditedAsset->GetPathName());

	CompileLogResults.Note(TEXT("Compile dialog"));

	if (!CompilerResultsTab.IsValid())
		CompilerResultsTab = TabManager->InvokeTab(FQaDSAssetEditorTabs::CompilerResultsID);

	CompilerResultsListing->ClearMessages();

	auto rootNode = GetRootNode();

	if (rootNode != NULL)
	{
		ResetCompilePhrase(rootNode);

		auto rootPhrase = Compile(rootNode);

		if (rootPhrase != NULL)
		{
			EditedAsset->RootNode = rootPhrase;
		}
		else
			CompileLogResults.Error(TEXT("Root node not found"));
	}
	else
		CompileLogResults.Error(TEXT("Root node not found"));

	CompileLogResults.EndEvent();
	CompilerResultsListing->AddMessages(CompileLogResults.Messages);
}

void FDialogAssetEditor::ResetCompilePhrase(UDialogEdGraphNode* Node)
{
	if (Node->CompileNode == NULL)
		return;

	Node->CompileNode = NULL;

	for (auto& child : Node->GetChildNodes())
	{
		auto childPhrase = Cast<UDialogEdGraphNode>(child);
		if (childPhrase)
			ResetCompilePhrase(childPhrase);
	}
}

UDialogNode* FDialogAssetEditor::Compile(UDialogEdGraphNode* node)
{
	if (node->CompileNode != NULL)
		return node->CompileNode;

	auto phraseNode = Cast<UDialogPhraseEdGraphNode>(node);
	auto rootNode = Cast<UDialogRootEdGraphNode>(node);
	auto subGraphNode = Cast<UDialogSubGraphEdGraphNode>(node);
	auto elseIfNode = Cast<UDialogElseIfEdGraphNode>(node);
	bool needUpdate = false;

	if (rootNode != NULL)
	{
		auto compileNode = NewObject<UDialogNode>((UObject*)EditedAsset);
		node->CompileNode = compileNode;
	}
	else if (subGraphNode != NULL)
	{
		auto compileNode = NewObject<UDialogSubGraphNode>((UObject*)EditedAsset);
		compileNode->TargetDialogAsset = subGraphNode->TargetDialogAsset;
		node->CompileNode = compileNode;
	}
	else if (elseIfNode != NULL)
	{
		auto compileNode = NewObject<UDialogElseIfNode>((UObject*)EditedAsset);
		compileNode->Conditions = elseIfNode->Conditions;
		node->CompileNode = compileNode;
	}
	else if (phraseNode != NULL)
	{
		auto compileNode = NewObject<UDialogPhraseNode>((UObject*)EditedAsset);
		node->CompileNode = compileNode;

		auto& data = phraseNode->Data;
		data.UID = *node->NodeGuid.ToString();

		compileNode->OwnerDialog = Cast<UDialogAsset>(EditedAsset);
		compileNode->Data = data;

		FString ErrorMessage;

		for (auto& Event : data.Action)
		{
			Event.OwnerNode = compileNode;

			if (!Event.Compile(ErrorMessage, needUpdate))
			{
				CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + data.Text.ToString() + "\""));
			}
		}

		for (auto& Condition : data.Predicate)
		{
			Condition.OwnerNode = compileNode;

			if (!Condition.Compile(ErrorMessage, needUpdate))
			{
				CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + data.Text.ToString() + "\""));
			}
		}
	}

	auto childs = node->GetChildNodes();
	childs.Sort([](auto& a, auto& b)
	{
		return a.GetOrder() < b.GetOrder();
	});

	bool first = true;
	EDialogPhraseSource source = EDialogPhraseSource::NPC;

	for (auto& child : childs)
	{
		auto childPhrase = Cast<UDialogPhraseEdGraphNode>(child);

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
		}

		node->CompileNode->Childs.Add(Compile(child));
	}

	if (needUpdate && PropertyEditor.IsValid())
	{
		for (auto& obj : PropertyEditor->GetSelectedObjects())
		{
			if (obj == node)
			{
				auto selected = GraphEditor->GetSelectedNodes();
				GraphEditor->ClearSelectionSet();

				for (auto n : selected.Array())
					GraphEditor->SetNodeSelection(Cast<UEdGraphNode>(n), true);

				break;
			}
		}
	}

	return node->CompileNode;
}

#undef LOCTEXT_NAMESPACE