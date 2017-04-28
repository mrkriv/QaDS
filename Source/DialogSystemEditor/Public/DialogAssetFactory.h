// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "Factories/Factory.h"
#include "DialogAssetFactory.generated.h"

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool CanCreateNew() const override;
};