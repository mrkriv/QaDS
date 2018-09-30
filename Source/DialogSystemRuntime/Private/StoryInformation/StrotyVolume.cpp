#include "DialogSystemRuntime.h"
#include "StrotyVolume.h"
#include "StoryInformationManager.h"
#include "StoryTriggerManager.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"
#include "GameFramework/Pawn.h"

void AStrotyVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	auto playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (playerCharacter != NULL && Other == playerCharacter)
	{
		Activate();
	}
}

void AStrotyVolume::Activate()
{
	UE_LOG(DialogModuleLog, Log, TEXT("Activate story volume %s"), *GetFName().ToString());

	if (RemoveKeys.Num() + GiveKeys.Num() > 0)
	{
		auto skm = UStoryKeyManager::GetStoryKeyManager();

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
		auto stm = UStoryTriggerManager::GetStoryTriggerManager();
		for (auto& trigger : ActivateTriggers)
		{
			stm->InvokeTrigger(trigger);
		}
	}

	if (!StartQuest.IsNull())
	{
		auto questProcesstor = UQuestProcessor::GetQuestProcessor();
		questProcesstor->StartQuest(StartQuest);
	}

	if (bDestroySelf)
	{
		Destroy();
	}
}