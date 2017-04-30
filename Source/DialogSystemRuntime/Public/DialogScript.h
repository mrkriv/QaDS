// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "DialogScript.generated.h"

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API ADialogScript : public AInfo
{
	GENERATED_BODY()

private:
	bool Trigger;

public:	
	ADialogScript();

	UPROPERTY()
	class UDialogImplementer* Implementer;

	UFUNCTION(BlueprintImplementableEvent)
	void ActivatePhrase();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTriggerChange(bool IsSet);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void SetTrigger();

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void ResetTrigger();

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void SetTriggerValue(bool IsSetNew);

	UFUNCTION(BlueprintPure, Category = "Dialog")
	bool GetTrigger();

	UFUNCTION(BlueprintPure, Category = "Dialog")
	AActor* GetPlayer();

	UFUNCTION(BlueprintPure, Category = "Dialog")
	AActor* GetInterlocutor();
};
