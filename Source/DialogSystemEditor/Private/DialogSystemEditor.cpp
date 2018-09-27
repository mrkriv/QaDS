#include "DialogSystemEditor.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "ISettingsModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "BrushSet.h"

#include "StoryKeyWindow.h"
#include "QaDSSettings.h"

#include "DialogEditorNodeFactory.h"
#include "DialogPhraseEventCustomization.h"
#include "PhraseNodeCustomization.h"
#include "DialogAssetTypeActions.h"
#include "DialogAssetEditor.h"

#include "QuestEditorNodeFactory.h"
#include "QuestStageEventCustomization.h"
#include "QuestStageCustomization.h"
#include "QuestAssetTypeActions.h"
#include "QuestAssetEditor.h"

DEFINE_LOG_CATEGORY(DialogModuleLog)

#define LOCTEXT_NAMESPACE "FDialogSystemModule"

void FDialogSystemEditorModule::StartupModule()
{
	FBrushSet::Register();
	FDialogCommands::Register();
	FQuestCommands::Register();

	auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Gameplay")), LOCTEXT("GameplayAssetCategory", "Gameplay"));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FQuestAssetTypeActions(AssetCategory)));
	
	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("DialogPhraseEdGraphNode", FOnGetDetailCustomizationInstance::CreateStatic(&FPhraseNodeDetails::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("DialogPhraseEvent", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogPhraseEventCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("DialogPhraseCondition", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDialogPhraseEventCustomization::MakeInstance));
	
	PropertyModule.RegisterCustomClassLayout("QuestStageEdGraphNode", FOnGetDetailCustomizationInstance::CreateStatic(&FQuestStageDetails::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("QuestStageEvent", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FQuestStageEventCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("QuestStageCondition", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FQuestStageEventCustomization::MakeInstance));

	auto& SettingsModule = FModuleManager::LoadModuleChecked<ISettingsModule>("Settings");
	SettingsModule.RegisterSettings("Project", "Plugins", "Dialog",
		LOCTEXT("RuntimeSettingsName", "Dialog Editor"),
		LOCTEXT("RuntimeSettingsDescription", "Dialog editor settings"),
		UQaDSSettings::StaticClass()->GetDefaultObject()
	);

	FEdGraphUtilities::RegisterVisualNodeFactory(MakeShareable(new FQuestEditorNodeFactory));
	FEdGraphUtilities::RegisterVisualNodeFactory(MakeShareable(new FDialogEditorNodeFactory));

	auto& MenuStructure = WorkspaceMenu::GetMenuStructure();
	auto developerCategory = MenuStructure.GetDeveloperToolsMiscCategory();
	auto& SpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner("StoryKeyWindow", FOnSpawnTab::CreateRaw(this, &FDialogSystemEditorModule::SpawnStoryKeyTab))
		.SetDisplayName(LOCTEXT("StoryKey", "Story Key"))
		.SetIcon(FSlateIcon("DialogSystem", "DialogSystem.StoryKeyIcon"));

	SpawnerEntry.SetGroup(developerCategory);
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
		PropertyModule.UnregisterCustomClassLayout("DialogPhraseNode");
		PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseEvent");
		PropertyModule.UnregisterCustomPropertyTypeLayout("DialogPhraseCondition");

		PropertyModule.UnregisterCustomClassLayout("QuestStageEdGraphNode");
		PropertyModule.UnregisterCustomPropertyTypeLayout("QuestStageEvent");
		PropertyModule.UnregisterCustomPropertyTypeLayout("QuestStageCondition");
	}

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(MakeShareable(new FDialogAssetTypeActions(AssetCategory)));
		AssetTools.UnregisterAssetTypeActions(MakeShareable(new FQuestAssetTypeActions(AssetCategory)));
	}
}

TSharedRef<SDockTab> FDialogSystemEditorModule::SpawnStoryKeyTab(const FSpawnTabArgs&)
{
	TSharedRef<SDockTab> tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	tab->SetContent(SNew(SStoryKeyWindow));

	return tab;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogSystemEditorModule, DialogSystemEditor)