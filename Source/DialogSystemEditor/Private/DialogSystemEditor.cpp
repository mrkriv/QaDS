#include "DialogSystemEditor.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "GraphPanelNodeFactory_Dialog.h"
#include "DialogPhraseEventCustomization.h"
#include "PhraseNodeCustomization.h"
#include "DialogAssetTypeActions.h"
#include "DialogCommands.h"
#include "BrushSet.h"

DEFINE_LOG_CATEGORY(DialogModuleLog)

#define LOCTEXT_NAMESPACE "FDialogSystemModule"

void FDialogSystemEditorModule::StartupModule()
{
	FBrushSet::Register();
	FDialogCommands::Register();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Gameplay")), LOCTEXT("GameplayAssetCategory", "Gameplay"));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("DialogPhraseCondition", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogPhraseEventCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("DialogPhraseEvent", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogPhraseEventCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("PhraseNode", FOnGetDetailCustomizationInstance::CreateStatic(&FPhraseNodeDetails::MakeInstance));

	TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory = MakeShareable(new FGraphPanelNodeFactory_Dialog);
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory);
}

void FDialogSystemEditorModule::ShutdownModule()
{
	FDialogCommands::Unregister();
	FBrushSet::Unregister();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseCondition");
	PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseEvent");
	PropertyModule.UnregisterCustomClassLayout("PhraseNode");

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	AssetTools.UnregisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogSystemEditorModule, DialogSystemEditor)