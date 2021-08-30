#include "DialogSystemRuntime.h"
#include "StrotyVolume.h"
#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"

bool AStrotyVolume::CanActivate(AActor* Other)
{
	if (CheckHasKeys.Num() + CheckDontHasKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager(this);

		for (auto& key : CheckHasKeys)
		{
			if (skm->DontHasKey(key))
				return false;
		}

		for (auto& key : CheckDontHasKeys)
		{
			if (skm->HasKey(key))
				return false;
		}
	}

	return true;
}

void AStrotyVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	auto playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (playerCharacter != NULL && Other == playerCharacter)
	{
		if (CanActivate(Other))
			Activate();
	}
}

void AStrotyVolume::Activate()
{
	UE_LOG(DialogModuleLog, Log, TEXT("Activate story volume %s"), *GetFName().ToString());

	if (RemoveKeys.Num() + GiveKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager(this);

		for (auto& key : GiveKeys)
		{
			skm->AddKey(key);
		}

		for (auto& key : RemoveKeys)
		{
			skm->RemoveKey(key);
		}
	}
	
	if (ActivateTriggers.Num() > 0)
	{
		auto stm = UStoryTriggerManager::GetStoryTriggerManager(this);
		for (auto& trigger : ActivateTriggers)
		{
			stm->InvokeTrigger(trigger);
		}
	}

	if (!StartQuest.IsNull())
	{
		auto questProcesstor = UQuestProcessor::GetQuestProcessor(this);
		questProcesstor->StartQuest(StartQuest);
	}

	if (bDestroySelf)
	{
		Destroy();
	}
}