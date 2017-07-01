// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "DialogNodeFactory.h"
#include "DialogPhraseEventCustomization.h"
#include "PhraseNodeCustomization.h"
#include "DialogAssetTypeActions.h"
#include "DialogAssetEditor.h"
#include "ISettingsModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "DialogSettings.h"
#include "DialogScript.h"
#include "BrushSet.h"

DEFINE_LOG_CATEGORY(DialogModuleLog)

#define LOCTEXT_NAMESPACE "FDialogSystemModule"

void FDialogSystemEditorModule::StartupModule()
{
	FBrushSet::Register();
	FDialogCommands::Register();

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Gameplay")), LOCTEXT("GameplayAssetCategory", "Gameplay"));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));
	
	PropertyModule.RegisterCustomPropertyTypeLayout("DialogPhraseCondition", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogPhraseEventCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("DialogPhraseEvent", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogPhraseEventCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("PhraseNode", FOnGetDetailCustomizationInstance::CreateStatic(&FPhraseNodeDetails::MakeInstance));

	SettingsModule.RegisterSettings("Project", "Plugins", "Dialog",
		LOCTEXT("RuntimeSettingsName", "Dialog Editor"),
		LOCTEXT("RuntimeSettingsDescription", "Dialog editor settings"),
		UDialogSettings::StaticClass()->GetDefaultObject()
	);

	TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory = MakeShareable(new FDialogNodeFactory);
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory);
}


void FDialogSystemEditorModule::ShutdownModule()
{
	FDialogCommands::Unregister();
	FBrushSet::Unregister();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	ISettingsModule& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");

	SettingsModule.UnregisterSettings("Project", "Plugins", "Dialog");

	PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseCondition");
	PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseEvent");
	PropertyModule.UnregisterCustomClassLayout("PhraseNode");

	AssetTools.UnregisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogSystemEditorModule, DialogSystemEditor)