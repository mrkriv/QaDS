// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "ModuleManager.h"
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"

DECLARE_LOG_CATEGORY_EXTERN(DialogStalkerInporterLog, All, All)

class FDialogStalkerInporterModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedRef<class SDockTab> SpawnTab(const class FSpawnTabArgs&);
};