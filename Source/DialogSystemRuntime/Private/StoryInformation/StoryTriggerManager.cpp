#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryTriggerManager.h"

UStoryTriggerManager* UStoryTriggerManager::Instance = NULL;

UStoryTriggerManager* UStoryTriggerManager::GetStoryTriggerManager(UObject* WorldContextObject)
{
	if (Instance == NULL)
		Instance = NewObject<UStoryTriggerManager>(WorldContextObject);
	
	return Instance;
}

void UStoryTriggerManager::BeginDestroy()
{
	Super::BeginDestroy();

	if (Instance == this)
		Instance = NULL;
}

void UStoryTriggerManager::InvokeTrigger(const FStoryTrigger& Trigger)
{
	OnTriggerInvoke.Broadcast(Trigger);

	// todo:: check contains TriggerName in setting
	// todo:: change FName to FTriggerName and add custom editor

	UE_LOG(DialogModuleLog, Log, TEXT("Invoke trigger %s"), *Trigger.TriggerName.ToString());
}
