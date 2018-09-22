#pragma once

#include "QaDSAssetEditor.h"
#include "QuestAsset.h"
#include "QuestEditorNodes.h"

class DIALOGSYSTEMEDITOR_API FQuestAssetEditor : public FQaDSAssetEditor
{
	UQuestAsset* EditedAsset;

public:
	void InitQuestAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UQuestAsset* Object);
	void BuildToolbar(FToolBarBuilder &builder);

protected:
	UObject* GetEditedAsset() const override;
	FName GetEditorName() const override;

	UEdGraph* CreateGraphFromAsset();
	UQuestEdGraphNode* GetRootNode();

	void Compile() override;
	void ResetCompilePhrase(UQuestEdGraphNode* Node);
	UQuestNode* Compile(UQuestEdGraphNode* Node);
};

struct DIALOGSYSTEMEDITOR_API FQuestCommands : public TCommands<FQuestCommands>
{
	TSharedPtr<FUICommandInfo> Compile;
	TSharedPtr<FUICommandInfo> Find;
	TSharedPtr<FUICommandInfo> Import;
	TSharedPtr<FUICommandInfo> Export;

	FQuestCommands()
		: TCommands<FQuestCommands>(TEXT("Quest Graph Commands"), FText::FromString("Quest Graph Commands"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;
};