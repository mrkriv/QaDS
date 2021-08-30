#include "DialogAssetEditor.h"

#include "AssetEditorManager.h"
#include "Editor.h"
#include "DialogGraphSchema.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "SNodePanel.h"
#include "DialogNodes.h"
#include "DialogEditorNodes.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/DebuggerCommands.h"
#include "BrushSet.h"
#include "QaDSGraphSchema.h"

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

void FDialogAssetEditor::InitDialogAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UDialogAsset* Object)
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
	ToolkitCommands->MapAction(FDialogCommands::Get().Compile, FExecuteAction::CreateSP(this, &FDialogAssetEditor::CompileExecute));
	ToolkitCommands->MapAction(FDialogCommands::Get().Find, FExecuteAction::CreateSP(this, &FDialogAssetEditor::OpenFindWindow));
	ToolkitCommands->MapAction(FDialogCommands::Get().Import, FExecuteAction::CreateSP(this, &FDialogAssetEditor::ImportExecute));
	ToolkitCommands->MapAction(FDialogCommands::Get().Export, FExecuteAction::CreateSP(this, &FDialogAssetEditor::ExportExecute));
	
	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, ToolkitCommands, FToolBarExtensionDelegate::CreateRaw(this, &FDialogAssetEditor::BuildToolbar));
	AddToolbarExtender(ToolbarExtender);

	InitAssetEditor(Mode, InitToolkitHost, DialogEditorAppName, GetDefaultLayout(), true, true, Object);

	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FDialogAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FDialogAssetEditor::BuildToolbar(FToolBarBuilder &builder)
{
	auto iconCompile = FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetEditor.ReimportAsset", "AssetEditor.ReimportAsset.Small");
	auto iconFind = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");
	auto iconExport = FSlateIcon(FEditorStyle::GetStyleSetName(), "DialogSystem.Export_48");
	auto iconImport = FSlateIcon(FEditorStyle::GetStyleSetName(), "DialogSystem.Import_48");

	builder.AddToolBarButton(FDialogCommands::Get().Compile, NAME_None, FText::FromString("Compile"), FText::FromString("Compile this dialog"), iconCompile, NAME_None);
	builder.AddSeparator(); 
	//builder.AddToolBarButton(FDialogCommands::Get().Find, NAME_None, FText::FromString("Find"), FText::FromString("Open find dialog"), iconFind, NAME_None);
	//builder.AddSeparator(); 
	builder.AddToolBarButton(FDialogCommands::Get().Export, NAME_None, FText::FromString("Export"), FText::FromString("Export graph to file"), iconImport, NAME_None);
	builder.AddToolBarButton(FDialogCommands::Get().Import, NAME_None, FText::FromString("Import"), FText::FromString("Imoprt graph from file"), iconExport, NAME_None);
	builder.AddSeparator();

	//todo:: applay FPlayWorldCommands to this menu 
	//FPlayWorldCommands::BuildToolbar(builder);
}

UDialogRootEdGraphNode* FDialogAssetEditor::GetRootNode()
{
	for (auto node : GraphEditor->GetCurrentGraph()->Nodes)
	{
		if (auto root = Cast<UDialogRootEdGraphNode>(node))
			return root;
	}

	return NULL;
}

UEdGraph* FDialogAssetEditor::CreateGraphFromAsset()
{
	auto CustGraph = NewObject<UEdGraph>(EditedAsset, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
	CustGraph->Schema = UDialogGraphSchema::StaticClass();

	auto root = FQaDSSchemaAction_NewNode::SpawnNodeFromTemplate<UDialogRootEdGraphNode>(CustGraph, NewObject<UDialogRootEdGraphNode>(), FVector2D::ZeroVector, false);
	root->AllocateDefaultPins();

	return CustGraph;
}

void FDialogAssetEditor::Compile()
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

UDialogNode* FDialogAssetEditor::Compile(UDialogEdGraphNode* node)
{
	if (node->IsCompile())
		return node->CompileNode;

	node->SetCompile();

	auto phraseNode = Cast<UDialogPhraseEdGraphNode>(node);
	auto rootNode = Cast<UDialogRootEdGraphNode>(node);
	auto subGraphNode = Cast<UDialogSubGraphEdGraphNode>(node);
	auto elseIfNode = Cast<UDialogElseIfEdGraphNode>(node);

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

			if (!Event.Compile(ErrorMessage))
			{
				CompileLogResults.Error(*(ErrorMessage + "\tIn node \"" + data.Text.ToString() + "\""));
			}
		}

		for (auto& Condition : data.Predicate)
		{
			Condition.OwnerNode = compileNode;

			if (!Condition.Compile(ErrorMessage))
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
		auto dialogPhrase = Cast<UDialogEdGraphNode>(child);

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

		if (dialogPhrase != NULL)
			node->CompileNode->Childs.Add(Compile(dialogPhrase));
	}
	
	return node->CompileNode;
}

#undef LOCTEXT_NAMESPACE