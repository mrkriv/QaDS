#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryTriggerManager.h"

UStoryTriggerManager* UStoryTriggerManager::GetStoryTriggerManager()
{
	TObjectIterator<UStoryTriggerManager> iter;
	return iter ? *iter : NewObject<UStoryTriggerManager>();
}

void UStoryTriggerManager::InvokeTrigger(const FStoryTrigger& Trigger)
{
	OnTriggerInvoke.Broadcast(Trigger);

	// todo:: check contains TriggerName in setting
	// todo:: change FName to FTriggerName and add custom editor

	UE_LOG(DialogModuleLog, Log, TEXT("Invoke trigger %s"), *Trigger.TriggerName.ToString());
}
