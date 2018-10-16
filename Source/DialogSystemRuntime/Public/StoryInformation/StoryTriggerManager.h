#pragma once

#include "EngineUtils.h"
#include "StoryTriggerManager.generated.h"

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FStoryTrigger
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TriggerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> Params;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStoryTriggerInvokeSignature, const FStoryTrigger&, Trigger);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UStoryTriggerManager : public UObject
{
	GENERATED_BODY()

	static UStoryTriggerManager* Instance;

public:
	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Triggers")
	FStoryTriggerInvokeSignature OnTriggerInvoke;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Triggers", meta = (WorldContext = "WorldContextObject"))
	static UStoryTriggerManager* GetStoryTriggerManager(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Triggers")
	void InvokeTrigger(const FStoryTrigger& Trigger);

	virtual void BeginDestroy() override;
};
