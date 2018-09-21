#pragma once

#include "Factories/Factory.h"
#include "QuestAssetFactory.generated.h"

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool CanCreateNew() const override;
};