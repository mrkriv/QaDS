// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "DialogScript.generated.h"

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API ADialogScript : public AInfo
{
	GENERATED_BODY()
	
public:	
	ADialogScript();

	UPROPERTY()
	class UDialogAsset* Dialog;

	UFUNCTION(BlueprintImplementableEvent)
	void ActivatePhrase();
};
