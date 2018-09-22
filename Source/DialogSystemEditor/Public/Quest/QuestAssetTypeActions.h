#pragma once

#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Developer/AssetTools/Public/AssetTypeActions_Base.h"

class DIALOGSYSTEMEDITOR_API FQuestAssetTypeActions : public FAssetTypeActions_Base
{
private:
	EAssetTypeCategories::Type AssetCategory;

public:

	FQuestAssetTypeActions(EAssetTypeCategories::Type InAssetCategory);

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;
	virtual UClass* GetSupportedClass() const override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }
	virtual uint32 GetCategories() override;
};
