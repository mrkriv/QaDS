#pragma once

#include "EngineUtils.h"
#include "StoryTriggerManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStoryTriggerInvokeSignature, const FName&, TriggerName, const TArray<FString>&, Params);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UStoryTriggerManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Gameplay|StoryKey")
	FStoryTriggerInvokeSignature OnTriggerInvoke;

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	static UStoryTriggerManager* GetStoryTriggerManager();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	void InvokeTrigger(const FName& TriggerName, const TArray<FString>& Params);
};
