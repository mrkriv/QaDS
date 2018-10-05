#pragma once

#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"
#include "Editor/UnrealEd/Public/Kismet2/CompilerResultsLog.h"
#include "Developer/MessageLog/Public/IMessageLogListing.h"
#include "Developer/MessageLog/Public/MessageLogModule.h"
#include "Runtime/CoreUObject/Public/Misc/NotifyHook.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "SlateBasics.h"
#include "Commands.h"
#include "EditorStyle.h"
#include "Runtime/Core/Public/Logging/TokenizedMessage.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"

class UEdGraph;
class UEdGraphNode;
class SGraphEditor;
class UQaDSEdGraphNode;
class FXmlFile;

struct DIALOGSYSTEMEDITOR_API FQaDSAssetEditorTabs
{
	static const FName DetailsID;
	static const FName CompilerResultsID;
	static const FName GraphEditorID;
	
private:
	FQaDSAssetEditorTabs() {}
};

class DIALOGSYSTEMEDITOR_API FQaDSAssetEditor : public FAssetEditorToolkit, public FNotifyHook
{
	UObject* EditedAsset;

public:
	~FQaDSAssetEditor();

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	void OpenFindWindow();
	void ImportExecute();
	void ExportExecute();
	void CompileExecute();

protected:
	TSharedPtr<class IMessageLogListing> CompilerResultsListing;
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<FUICommandList> GraphEditorCommands;
	TSharedPtr<IDetailsView> PropertyEditor;
	TSharedPtr<FUICommandList> MyToolBarCommands;
	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<SDockTab> CompilerResultsTab;
	TSharedPtr<SDockTab> KeysTab;
	TSharedPtr<SDockTab> KeyManagerTab;
	FDelegateHandle OnGraphChangedDelegateHandle;
	FDelegateHandle OnPropertyChangedDelegateHandle;
	FCompilerResultsLog CompileLogResults;
	bool bGraphStateChanged;
	UEdGraph* EdGraph;

	TSharedRef<SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_CompilerResults(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<FTabManager::FLayout> GetDefaultLayout();

	void OnGraphChanged(const FEdGraphEditAction& Action);
	void OnPropertyChanged(const FPropertyChangedEvent& Event);
	void SelectAllNodes();
	bool CanSelectAllNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNode(UEdGraphNode* Node);
	bool CanDeleteNodes() const;
	void DeleteNodes(const TArray<UEdGraphNode*>& NodesToDelete);
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;
	void CutSelectedNodes();
	bool CanCutNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const;
	void DeleteSelectedDuplicatableNodes();
	void OnSelectedNodesChanged(const TSet<UObject*>& NewSelection);
	void OnNodeDoubleClicked(UEdGraphNode* Node); 
	void ResetCompilePhrase(UQaDSEdGraphNode* Node);

	virtual void Compile() = 0;
};