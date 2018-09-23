#include "DialogSystemEditor.h"
#include "QuestAssetEditor.h"
#include "Editor.h"
#include "QuestGraphSchema.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "SDockTab.h"
#include "SNodePanel.h"
#include "SGraphPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Views/SListView.h"
#include "QuestNodes.h"
#include "QuestEditorNodes.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/DebuggerCommands.h"
#include "ScopedTransaction.h"
#include "QaDSGraphSchema.h"

#define LOCTEXT_NAMESPACE "QuestGraph"

const FName QuestEditorAppName(TEXT("QuestEditorApp"));

void FQuestCommands::RegisterCommands()
{
	UI_COMMAND(Compile, "Compile", "Compile this quest graph", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Find, "Find", "Open find quest", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Import, "Import", "Imoprt graph from file", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Export, "Export", "Export graph to file", EUserInterfaceActionType::Button, FInputChord());
}

FName FQuestAssetEditor::GetEditorName() const
{
	return TEXT("Quest"); 
}

UObject* FQuestAssetEditor::GetEditedAsset() const
{
	return EditedAsset;
}

void FQuestAssetEditor::InitQuestAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UQuestAsset* Object)
{
	FAssetEditorManager::Get().CloseOtherEditors(Object, this);
	EditedAsset = Object;

	if (EditedAsset->UpdateGraph == NULL)
		EditedAsset->UpdateGraph = CreateGraphFromAsset();
	
	GraphEditor = CreateGraphEditorWidget(EditedAsset->UpdateGraph);

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("QuestEditor_Layout")
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

	ToolkitCommands = MakeShareable(new FUICommandList);
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
	ToolkitCommands->MapAction(FQuestCommands::Get().Compile, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::CompileExecute));
	ToolkitCommands->MapAction(FQuestCommands::Get().Find, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::OpenFindWindow));
	ToolkitCommands->MapAction(FQuestCommands::Get().Import, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::ImportExecute));
	ToolkitCommands->MapAction(FQuestCommands::Get().Export, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::ExportExecute));
	
	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FQuestAssetEditor::BuildToolbar));
	AddToolbarExtender(ToolbarExtender);

	InitAssetEditor(Mode, InitToolkitHost, QuestEditorAppName, StandaloneDefaultLayout, true, true, Object);

	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FQuestAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FQuestAssetEditor::BuildToolbar(FToolBarBuilder &builder)
{
	auto iconCompile = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");
	auto iconFind = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");
	auto iconExport = FSlateIcon(FEditorStyle::GetStyleSetName(), "");
	auto iconImport = FSlateIcon(FEditorStyle::GetStyleSetName(), "");

	builder.AddSeparator();
	builder.AddToolBarButton(FQuestCommands::Get().Compile, NAME_None, FText::FromString("Compile"), FText::FromString("Compile this Quest"), iconCompile, NAME_None);
	//builder.AddSeparator(); 
	//builder.AddToolBarButton(FQuestCommands::Get().Find, NAME_None, FText::FromString("Find"), FText::FromString("Open find Quest"), iconFind, NAME_None);
	//builder.AddSeparator(); 
	//builder.AddToolBarButton(FQuestCommands::Get().Export, NAME_None, FText::FromString("Export"), FText::FromString("Export graph to file"), iconImport, NAME_None);
	//builder.AddToolBarButton(FQuestCommands::Get().Import, NAME_None, FText::FromString("Import"), FText::FromString("Imoprt graph from file"), iconExport, NAME_None);
	//builder.AddSeparator();

	FPlayWorldCommands::BuildToolbar(builder);
}

UQuestEdGraphNode* FQuestAssetEditor::GetRootNode()
{
	for (auto node : GraphEditor->GetCurrentGraph()->Nodes)
	{
		if (auto root = Cast<UQuestEdGraphNode>(node))
			return root;
	}

	return NULL;
}

UEdGraph* FQuestAssetEditor::CreateGraphFromAsset()
{
	auto CustGraph = NewObject<UEdGraph>(EditedAsset, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
	CustGraph->Schema = UQuestGraphSchema::StaticClass();

	auto root = FQaDSSchemaAction_NewNode::SpawnNodeFromTemplate<UQaDSEdGraphNode>(CustGraph, NewObject<UQuestRootEdGraphNode>(), FVector2D::ZeroVector, false);
	root->AllocateDefaultPins();

	return CustGraph;
}

void FQuestAssetEditor::ResetCompilePhrase(UQuestEdGraphNode* Node)
{
	if (Node->CompileNode == NULL)
		return;

	Node->CompileNode = NULL;

	for (auto& child : Node->GetChildNodes())
	{
		auto childPhrase = Cast<UQuestEdGraphNode>(child);
		if (childPhrase)
			ResetCompilePhrase(childPhrase);
	}
}

void FQuestAssetEditor::Compile()
{
	auto rootNode = GetRootNode();
	if (rootNode == NULL)
	{
		CompileLogResults.Error(TEXT("Root node not found"));
		return;
	}

	ResetCompilePhrase(rootNode);
	EditedAsset->RootNode = Compile(rootNode);
}

UQuestNode* FQuestAssetEditor::Compile(UQuestEdGraphNode* node)
{
	if (node->CompileNode != NULL)
		return node->CompileNode;

	auto stageNode = Cast<UQuestStageEdGraphNode>(node);
	auto endNode = Cast<UQuestEndEdGraphNode>(node);
	auto rootNode = Cast<UQuestRootEdGraphNode>(node);
	bool needUpdate = false;

	if (rootNode != NULL)
	{
		node->CompileNode = NewObject<UQuestNode>(EditedAsset);
	}
	else if (endNode != NULL)
	{
		auto compileNode = NewObject<UQuestEndNode>(EditedAsset);
		compileNode->IsSuccesEnd = endNode->IsSuccesEnd;
		node->CompileNode = compileNode;
	}
	else if (stageNode != NULL)
	{
		auto compileNode = NewObject<UQuestNode>((UObject*)EditedAsset);
		node->CompileNode = compileNode;

		compileNode->OwnerQuest = Cast<UQuestAsset>(EditedAsset);
		compileNode->Stage = stageNode->Stage;

		FString ErrorMessage;

		for (auto& Event : compileNode->Stage.Action)
		{
			//todo:: create action for quest
			//Event.OwnerNode = compileNode;

			if (!Event.Compile(ErrorMessage, needUpdate))
			{
				CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + compileNode->Stage.SystemName.ToString() + "\""));
			}
		}

		for (auto& Condition : compileNode->Stage.Predicate)
		{
			//Condition.OwnerNode = compileNode;

			if (!Condition.Compile(ErrorMessage, needUpdate))
			{
				CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + compileNode->Stage.SystemName.ToString() + "\""));
			}
		}
	}

	auto childs = node->GetChildNodes();
	childs.Sort([](auto& a, auto& b)
	{
		return a.GetOrder() < b.GetOrder();
	});

	for (auto& child : childs)
	{
		auto questNode = Cast<UQuestEdGraphNode>(child);
		if (questNode != NULL)
			node->CompileNode->Childs.Add(Compile(questNode));
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