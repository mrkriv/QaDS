// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogStalkerInporterModule.h"
#include "ImportWindow.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Framework/Docking/TabManager.h"

DEFINE_LOG_CATEGORY(DialogStalkerInporterLog)

#define LOCTEXT_NAMESPACE "DialogStalkerInporterModule"

void FDialogStalkerInporterModule::StartupModule()
{
	auto& MenuStructure = WorkspaceMenu::GetMenuStructure();
	auto developerCategory = MenuStructure.GetDeveloperToolsMiscCategory();
	
	FTabSpawnerEntry& SpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner("DialogStalkerInporter", FOnSpawnTab::CreateRaw(this, &FDialogStalkerInporterModule::SpawnTab))
		.SetDisplayName(LOCTEXT("Inporter", "STALKER dialog importer"))
		.SetTooltipText(LOCTEXT("InporterTooltipText", "Import dialogs from STALKER resurces"));

	SpawnerEntry.SetGroup(developerCategory);
}

void FDialogStalkerInporterModule::ShutdownModule()
{
}

TSharedRef<SDockTab> FDialogStalkerInporterModule::SpawnTab(const FSpawnTabArgs&)
{
	TSharedRef<SDockTab> tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	tab->SetContent(SNew(SImportWindow));

	return tab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDialogStalkerInporterModule, DialogStalkerInporter)