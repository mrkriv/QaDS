#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryTriggerManager.h"

UStoryTriggerManager* UStoryTriggerManager::GetStoryTriggerManager()
{
	for (TObjectIterator<UStoryTriggerManager> Itr; Itr;)
		return *Itr;

	return NewObject<UStoryTriggerManager>();
}

void UStoryTriggerManager::InvokeTrigger(const FName& TriggerName, const TArray<FString>& Params)
{
	OnTriggerInvoke.Broadcast(TriggerName, Params);

	// todo:: check contains TriggerName in setting
	// todo:: change FName to FTriggerName and add custom editor

	UE_LOG(DialogModuleLog, Log, TEXT("Invoke trigger %s"), *TriggerName.ToString());
}
