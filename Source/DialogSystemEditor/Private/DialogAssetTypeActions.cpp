// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "DialogAssetTypeActions.h"
#include "DialogAsset.h"
#include "DialogAssetEditor.h"

FDialogAssetTypeActions::FDialogAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: AssetCategory(InAssetCategory)
{
}

FText FDialogAssetTypeActions::GetName() const
{
	return FText::FromString("Dialog");
}

FColor FDialogAssetTypeActions::GetTypeColor() const
{
	return FColor::Blue;
}

void FDialogAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto It = InObjects.CreateConstIterator(); It; ++It)
	{
		UDialogAsset* obj = Cast<UDialogAsset>(*It);
		if (obj)
		{
			TSharedRef<FDialogAssetEditor> NewCustEditor(new FDialogAssetEditor());
			NewCustEditor->InitDialogAssetEditor(Mode, EditWithinLevelEditor, obj);
		}
	}
}

UClass* FDialogAssetTypeActions::GetSupportedClass() const
{
	return UDialogAsset::StaticClass();
}

uint32 FDialogAssetTypeActions::GetCategories()
{
	return AssetCategory;
}