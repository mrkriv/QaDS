#include "DialogSystemRuntime.h"
#include "DialogProcessor.h"
#include "DialogScript.h"
#include "DialogPhrase.h"
#include "DialogNodes.h"
#include "DialogAsset.h"
#include "StoryInformationManager.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

UDialogProcessor* UDialogProcessor::CreateDialogProcessor(UDialogAsset* DialogAsset, AActor* InNPC)
{
	if (DialogAsset == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Dialog asset can not be empty"));
		return NULL;
	}

	if (DialogAsset->RootNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Dialog asset '%s' not have root node"), *DialogAsset->GetPathName());
		return NULL;
	}

	if (InNPC == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("NPC is null"));
		return NULL;
	}

	auto impl = NewObject<UDialogProcessor>(InNPC->GetWorld());
	impl->NPC = InNPC;
	impl->SetDialogAsset(DialogAsset);

	return impl;
}

void UDialogProcessor::StartDialog()
{
	StoryKeyManager = UStoryKeyManager::GetStoryKeyManager();
	SetCurrentNode(Asset->RootNode);
}

void UDialogProcessor::SetDialogAsset(UDialogAsset* NewDialogAsset)
{
	Asset = NewDialogAsset;

	if (Asset->DialogScriptClass == NULL)
	{
		DialogScript = NULL;
	}
	else if (DialogScript == NULL || !DialogScript->IsA(Asset->DialogScriptClass))
	{
		DialogScript = NPC->GetWorld()->SpawnActor<ADialogScript>(Asset->DialogScriptClass, FTransform());
		DialogScript->Implementer = this;
	}
}

void UDialogProcessor::SetCurrentNode(UDialogNode* node)
{
	IsPlayerNext = false;
	CurrentNode = node;
	NextNodes.Reset();

	for (auto childNode : CurrentNode->Childs)
	{
		if (!childNode->Check(this))
			continue;

		NextNodes.Append(childNode->GetNextPhrases(this));

		auto phrase = Cast<UDialogPhraseNode>(childNode);
		if (phrase)
		{
			IsPlayerNext = phrase->Data.Source == EDialogPhraseSource::Player;
		}
	}

	if (NextTimerHandle.IsValid() && NPC != NULL)
	{
		NPC->GetWorldTimerManager().ClearTimer(NextTimerHandle);
	}

	node->Invoke(this);
}

void UDialogProcessor::Next(FName PhraseUID)
{
	for (auto node : NextNodes)
	{
		if (node->Data.UID == PhraseUID) 
		{
			SetCurrentNode(node);
			return;
		}
	}

	UE_LOG(DialogModuleLog, Error, TEXT("Not found phrase UID %s"), *PhraseUID.ToString());
	EndDialog();
}

void UDialogProcessor::DelayNext()
{
	if (NextTimerHandle.IsValid())
		NPC->GetWorldTimerManager().ClearTimer(NextTimerHandle);

	if (NPC == NULL)
	{
		UE_LOG(DialogModuleLog, Warning, TEXT("NPC is null, phrase delay has ben skip"));
		OnTimerTick();
	}

	float delay = GetPhraseDuration();
	if (delay > 0)
	{
		NPC->GetWorldTimerManager().SetTimer(NextTimerHandle, this, &UDialogProcessor::OnTimerTick, delay, false);
	}
	else
	{
		OnTimerTick();
	}
}

void UDialogProcessor::OnTimerTick()
{
	if (NextNodes.Num() > 0)
	{
		Next(NextNodes[0]->Data.UID);
	}
	else
	{
		EndDialog();
	}
}

float UDialogProcessor::GetPhraseDuration()
{
	auto phraseNode = Cast<UDialogPhraseNode>(CurrentNode);

	if (phraseNode == NULL)
		return 0;

	if (!phraseNode->Data.AutoTime)
	{
		return phraseNode->Data.PhraseManualTime;
	}
	else if (phraseNode->Data.Sound != NULL)
	{
		return phraseNode->Data.Sound->Duration;
	}
	else
	{
		return 0;
	}
}

void UDialogProcessor::EndDialog()
{
	if (DialogScript != NULL)
		DialogScript->Destroy();

	OnEndDialog.Broadcast();
}