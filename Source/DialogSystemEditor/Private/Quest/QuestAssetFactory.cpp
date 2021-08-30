#include "QuestAssetFactory.h"
#include "QuestAsset.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

UQuestAssetFactory::UQuestAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UQuestAsset::StaticClass();
}

UObject* UQuestAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UQuestAsset>(InParent, Class, Name, Flags | RF_Transactional);
}

bool UQuestAssetFactory::CanCreateNew() const
{
	return true;
}