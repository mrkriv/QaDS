// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "DialogProcessor.h"
#include "DialogScript.h"
#include "DialogPhrase.h"
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

	auto impl = NewObject<UDialogProcessor>(InNPC->GetWorld());
	impl->NPC = InNPC;
	impl->Asset = DialogAsset;

	if (DialogAsset->DialogScriptClass != NULL)
	{
		impl->DialogScript = InNPC->GetWorld()->SpawnActor<ADialogScript>(DialogAsset->DialogScriptClass, FTransform());
		impl->DialogScript->Implementer = impl;
	}

	return impl;
}

void UDialogProcessor::StartDialog()
{
	SetCurrentNode(Asset->RootNode);
}

void UDialogProcessor::SetCurrentNode(UDialogPhrase* node)
{
	bool found = false;
	IsPlayerNext = false;

	NextNodes.Reset();
	CurrentNode = node;

	for (auto phraseNode : CurrentNode->GetChilds())
	{
		auto phrase = Cast<UDialogPhrase>(phraseNode);
		if (phrase && phrase->Check(this))
		{
			if (!found)
			{
				IsPlayerNext = phrase->Data.Source == EDialogPhraseSource::Player;
			}
			else if (IsPlayerNext != (phrase->Data.Source == EDialogPhraseSource::Player))
			{
				UE_LOG(DialogModuleLog, Error, TEXT("Invalid graph: Phrase cannot simultaneously refer to phrases of different types"));
				return;
			}

			NextNodes.Add(phrase);
			found = true;
		}
	}

	InvokeNode(CurrentNode);
}

void UDialogProcessor::InvokeNode(UDialogPhrase* node)
{
	if (NextTimerHandle.IsValid() && NPC != NULL)
		NPC->GetWorldTimerManager().ClearTimer(NextTimerHandle);

	node->Invoke(this);

	if (node->Data.Source == EDialogPhraseSource::Player)
	{
		OnShowPlayerPhrase.Broadcast(node->Data);
	}
	else
	{
		OnShowNPCPhrase.Broadcast(node->Data);
	}

	if (IsPlayerNext)
	{
		TArray<FDialogPhraseShortInfo> playerPhrases;

		for (auto nextNode : NextNodes)
		{
			FDialogPhraseShortInfo answerInfo;
			answerInfo.Text = nextNode->Data.Text;
			answerInfo.UID = nextNode->Data.UID;

			playerPhrases.Add(answerInfo);
		}

		OnChangePhraseVariant.Broadcast(playerPhrases);
	}
	else 
	{
		DelayNext();
	}
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
		OnNextTimer();
	}

	float delay = GetPhraseTime();
	if (delay > 0)
	{
		NPC->GetWorldTimerManager().SetTimer(NextTimerHandle, this, &UDialogProcessor::OnNextTimer, delay, false);
	}
	else
	{
		OnNextTimer();
	}
}

void UDialogProcessor::OnNextTimer()
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

float UDialogProcessor::GetPhraseTime()
{
	if (!CurrentNode->Data.AutoTime) 
	{
		return CurrentNode->Data.PhraseManualTime;
	}
	else if (CurrentNode->Data.Sound != NULL) 
	{
		return CurrentNode->Data.Sound->Duration;
	}
	else
	{
		return 0;

		/*if (CurrentNode->Data.Source == EDialogPhraseSource::Player)
			return 0.0f;

		int len = CurrentNode->Data.Text.ToString().Len();
		return .2f + len * 0.05f;*/
	}
}

void UDialogProcessor::EndDialog()
{
	if (DialogScript != NULL)
		DialogScript->Destroy();

	OnEndDialog.Broadcast();
}