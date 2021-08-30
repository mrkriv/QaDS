#include "QuestAssetEditor.h"
#include "Editor.h"
#include "QuestGraphSchema.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "SNodePanel.h"
#include "QuestNode.h"
#include "QuestEditorNodes.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/DebuggerCommands.h"
#include "BrushSet.h"
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

void FQuestAssetEditor::InitQuestAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UQuestAsset* Object)
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorSubsystem->CloseOtherEditors(Object, this);
	
	EditedAsset = Object;

	if (EditedAsset->UpdateGraph == NULL)
		EditedAsset->UpdateGraph = CreateGraphFromAsset();
	
	GraphEditor = CreateGraphEditorWidget(EditedAsset->UpdateGraph);
	EdGraph = EditedAsset->UpdateGraph;

	ToolkitCommands = MakeShareable(new FUICommandList);
	ToolkitCommands->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());
	ToolkitCommands->MapAction(FQuestCommands::Get().Compile, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::CompileExecute));
	ToolkitCommands->MapAction(FQuestCommands::Get().Find, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::OpenFindWindow));
	ToolkitCommands->MapAction(FQuestCommands::Get().Import, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::ImportExecute));
	ToolkitCommands->MapAction(FQuestCommands::Get().Export, FExecuteAction::CreateSP(this, &FQaDSAssetEditor::ExportExecute));
	
	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FQuestAssetEditor::BuildToolbar));
	AddToolbarExtender(ToolbarExtender);

	InitAssetEditor(Mode, InitToolkitHost, QuestEditorAppName, GetDefaultLayout(), true, true, Object);

	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FQuestAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FQuestAssetEditor::BuildToolbar(FToolBarBuilder &builder)
{
	auto iconCompile = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");
	auto iconFind = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");
	auto iconExport = FSlateIcon(FEditorStyle::GetStyleSetName(), "DialogSystem.Export_48");
	auto iconImport = FSlateIcon(FEditorStyle::GetStyleSetName(), "DialogSystem.Import_48");

	builder.AddSeparator();
	builder.AddToolBarButton(FQuestCommands::Get().Compile, NAME_None, FText::FromString("Compile"), FText::FromString("Compile this Quest"), iconCompile, NAME_None);
	//builder.AddSeparator(); 
	//builder.AddToolBarButton(FQuestCommands::Get().Find, NAME_None, FText::FromString("Find"), FText::FromString("Open find Quest"), iconFind, NAME_None);
	builder.AddSeparator(); 
	builder.AddToolBarButton(FQuestCommands::Get().Export, NAME_None, FText::FromString("Export"), FText::FromString("Export graph to file"), iconImport, NAME_None);
	builder.AddToolBarButton(FQuestCommands::Get().Import, NAME_None, FText::FromString("Import"), FText::FromString("Imoprt graph from file"), iconExport, NAME_None);
	builder.AddSeparator();

	//todo:: applay FPlayWorldCommands to this menu 
	//FPlayWorldCommands::BuildToolbar(builder);
}

UQuestRootEdGraphNode* FQuestAssetEditor::GetRootNode()
{
	for (auto node : GraphEditor->GetCurrentGraph()->Nodes)
	{
		if (auto root = Cast<UQuestRootEdGraphNode>(node))
			return root;
	}

	return NULL;
}

UEdGraph* FQuestAssetEditor::CreateGraphFromAsset()
{
	auto CustGraph = NewObject<UEdGraph>(EditedAsset, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
	CustGraph->Schema = UQuestGraphSchema::StaticClass();

	auto root = FQaDSSchemaAction_NewNode::SpawnNodeFromTemplate<UQuestRootEdGraphNode>(CustGraph, NewObject<UQuestRootEdGraphNode>(), FVector2D::ZeroVector, false);
	root->AllocateDefaultPins();

	return CustGraph;
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

	EditedAsset->Nodes.Reset();
	EditedAsset->RootNode = Compile(rootNode);
}

FGuid FQuestAssetEditor::Compile(UQaDSEdGraphNode* node)
{
	if (node->IsCompile())
		return node->NodeGuid;

	node->SetCompile();

	FQuestStageInfo stage;
	stage.UID = node->NodeGuid;

	auto stageNode = Cast<UQuestStageEdGraphNode>(node);
	if (stageNode != NULL)
	{
		stageNode->Stage.UID = node->NodeGuid;
		stage = stageNode->Stage;

		FString ErrorMessage;
		for (auto& Event : stage.Action)
		{
			if (!Event.Compile(EditedAsset, ErrorMessage))
			{
				CompileLogResults.Error(*(ErrorMessage));
			}
		}

		for (auto& Condition : stage.FailedPredicate)
		{
			if (!Condition.Compile(EditedAsset, ErrorMessage))
			{
				CompileLogResults.Error(*(ErrorMessage));
			}
		}

		for (auto& Condition : stage.WaitPredicate)
		{
			if (!Condition.Compile(EditedAsset, ErrorMessage))
			{
				CompileLogResults.Error(*(ErrorMessage));
			}
		}

		for (auto& Condition : stage.Predicate)
		{
			if (!Condition.Compile(EditedAsset, ErrorMessage))
			{
				CompileLogResults.Error(*(ErrorMessage));
			}
		}
	}

	auto childs = node->GetChildNodes();
	childs.Sort([](auto& a, auto& b)
	{
		return a.GetOrder() < b.GetOrder();
	});

	FQuestStageJoin joins;
	for (auto& child : childs)
	{
		joins.UIDs.Add(Compile(child));
	}

	EditedAsset->Nodes.Add(node->NodeGuid, stage);
	EditedAsset->Joins.Add(node->NodeGuid, joins);

	return node->NodeGuid;
}


#undef LOCTEXT_NAMESPACEw