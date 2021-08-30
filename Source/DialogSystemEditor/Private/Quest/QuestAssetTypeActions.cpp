#include "QuestAssetTypeActions.h"
#include "QuestAsset.h"
#include "QuestAssetEditor.h"

FQuestAssetTypeActions::FQuestAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: AssetCategory(InAssetCategory)
{
}

FText FQuestAssetTypeActions::GetName() const
{
	return FText::FromString("Quest");
}

FColor FQuestAssetTypeActions::GetTypeColor() const
{
	return FColor::Green;
}

void FQuestAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto It = InObjects.CreateConstIterator(); It; ++It)
	{
		UQuestAsset* obj = Cast<UQuestAsset>(*It);
		if (obj)
		{
			TSharedRef<FQuestAssetEditor> NewCustEditor(new FQuestAssetEditor());
			NewCustEditor->InitQuestAssetEditor(Mode, EditWithinLevelEditor, obj);
		}
	}
}

UClass* FQuestAssetTypeActions::GetSupportedClass() const
{
	return UQuestAsset::StaticClass();
}

uint32 FQuestAssetTypeActions::GetCategories()
{
	return AssetCategory;
}
