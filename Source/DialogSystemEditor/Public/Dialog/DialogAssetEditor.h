#pragma once

#include "QaDSAssetEditor.h"
#include "DialogAsset.h"
#include "DialogEditorNodes.h"

class UDialogPhraseNode;
class UDialogEdGraphNode;
class UDialogRootEdGraphNode;

class DIALOGSYSTEMEDITOR_API FDialogAssetEditor : public FQaDSAssetEditor
{
public:
	void InitDialogAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UDialogAsset* Object);
	void BuildToolbar(FToolBarBuilder &builder);

protected:
	UDialogAsset* EditedAsset;

	UObject* GetEditedAsset() const override;
	void CompileExecute() override;
	FName GetEditorName() const override;
	virtual FString ExportToXml() override;
	virtual void ImportFromXml(FXmlFile* xml) override;

	UEdGraph* CreateGraphFromAsset();
	UDialogRootEdGraphNode* GetRootNode();
	void ResetCompilePhrase(UDialogEdGraphNode* Node);
	UDialogNode* Compile(UDialogEdGraphNode* Node);
};

struct DIALOGSYSTEMEDITOR_API FDialogCommands : public TCommands<FDialogCommands>
{
	TSharedPtr<FUICommandInfo> Compile;
	TSharedPtr<FUICommandInfo> Find;
	TSharedPtr<FUICommandInfo> Import;
	TSharedPtr<FUICommandInfo> Export;

	FDialogCommands()
		: TCommands<FDialogCommands>(TEXT("Dialog Graph Commands"), FText::FromString("Dialog Graph Commands"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;
};