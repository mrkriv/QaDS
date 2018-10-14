#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryTriggerManager.h"

UStoryTriggerManager* UStoryTriggerManager::Instance;

UStoryTriggerManager* UStoryTriggerManager::CreateTriggerManager(UObject* WorldContextObject)
{
	Instance = NewObject<UStoryTriggerManager>(WorldContextObject);
	return Instance;
}

UStoryTriggerManager* UStoryTriggerManager::GetStoryTriggerManager()
{
	return Instance;
}

void UStoryTriggerManager::InvokeTrigger(const FStoryTrigger& Trigger)
{
	OnTriggerInvoke.Broadcast(Trigger);

	// todo:: check contains TriggerName in setting
	// todo:: change FName to FTriggerName and add custom editor

	UE_LOG(DialogModuleLog, Log, TEXT("Invoke trigger %s"), *Trigger.TriggerName.ToString());
}
