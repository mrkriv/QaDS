// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
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

	auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	auto& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

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

	if (FModuleManager::Get().IsModuleLoaded("Settings"))
	{
		auto& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
		SettingsModule.UnregisterSettings("Project", "Plugins", "Dialog");
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseCondition");
		PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseEvent");
		PropertyModule.UnregisterCustomClassLayout("PhraseNode");
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogSystemEditorModule, DialogSystemEditor)