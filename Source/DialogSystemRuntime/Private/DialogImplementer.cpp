// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "DialogPhrase.h"
#include "DialogImplementer.h"
#include "DialogScript.h"
#include "StoryInformationManager.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

UDialogImplementer* UDialogImplementer::ImplementDialog(UDialogAsset* DialogAsset, AActor* InInterlocutor)
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

	UDialogImplementer* impl = NewObject<UDialogImplementer>(InInterlocutor->GetWorld());
	impl->Init(DialogAsset, InInterlocutor);

	if (impl->NextNodes.Num() == 0)
		return NULL;

	return impl;
}

void UDialogImplementer::Init(UDialogAsset* DialogAsset, AActor* InInterlocutor)
{
	Interlocutor = InInterlocutor;
	Asset = DialogAsset;
	isInit = false;

	bool isPlayerNext;
	GetValidPhrases(NextNodes, DialogAsset->RootNode->GetChilds(), isPlayerNext);

	if (Asset->DialogScriptClass != NULL)
	{
		DialogScript = InInterlocutor->GetWorld()->SpawnActor<ADialogScript>(Asset->DialogScriptClass, FTransform());
		DialogScript->Implementer = this;
	}
}

void UDialogImplementer::CallEndDialog()
{
	if (DialogScript != NULL)
		DialogScript->Destroy();

	OnEndDialog.Broadcast();
}

void UDialogImplementer::Next(int PhraseIndex)
{
	if (NextTimerHandle.IsValid() && Interlocutor != NULL)
		Interlocutor->GetWorldTimerManager().ClearTimer(NextTimerHandle);

	if (isInit)
	{
		if (CurrentNode == NULL)
			return;

		if (NextNodes.Num() == 0)
		{
			CallEndDialog();
			CurrentNode = NULL;
			return;
		}

		if (PhraseIndex < 0 || PhraseIndex >= NextNodes.Num())
		{
			UE_LOG(DialogModuleLog, Error, TEXT("Invalid phrase index"));
			CallEndDialog();
			return;
		}
	}
	else
		isInit = true;

	CurrentNode = NextNodes[PhraseIndex];
	CurrentNode->Invoke(this);

	bool isPlayerNext;
	GetValidPhrases(NextNodes, CurrentNode->GetChilds(), isPlayerNext);

	if (CurrentNode->Data.Source == EDialogPhraseSource::Player)
	{
		if (isPlayerNext)
		{
			UE_LOG(DialogModuleLog, Error, TEXT("Invalid graph: Phrase of the player can not point to another player phrase"));
			CallEndDialog();
			return;
		}

		OnActivatePhrase.Broadcast(CurrentNode->Data);
		if (DialogScript != NULL)
			DialogScript->ActivatePhrase();

		DelayNext();
	}
	else
	{
		TArray<FDialogAnswerInfo> playerPhrases;

		if (isPlayerNext)
		{
			for (auto& node : NextNodes)
			{
				FDialogAnswerInfo answerInfo;
				answerInfo.Text = node->Data.Text;
				answerInfo.Important = node->Data.Important;
				answerInfo.Read = 
					node->UID.IsValid() &&
					UStoryKeyManager::HasKey(*(Asset->Name.ToString() + node->UID.ToString()), EStoryKeyTypes::DialogPhrases);

				playerPhrases.Add(answerInfo);
			}
		}
		else
			DelayNext();

		OnActivatePhrase.Broadcast(CurrentNode->Data);
		
		if (DialogScript != NULL)
			DialogScript->ActivatePhrase();

		OnUpdateAnswer.Broadcast(playerPhrases);
	}
}

void UDialogImplementer::DelayNext()
{
	if (NextTimerHandle.IsValid())
		Interlocutor->GetWorldTimerManager().ClearTimer(NextTimerHandle);

	if (Interlocutor == NULL || !EnableTimer)
		Next(0);

	float delay = GetPhraseTime();
	if (delay != 0)
		Interlocutor->GetWorldTimerManager().SetTimer(NextTimerHandle, this, &UDialogImplementer::OnNextTimer, delay, false);
	else
		Next(0);
}

float UDialogImplementer::GetPhraseTime()
{
	if (!CurrentNode->Data.AutoTime)
		return CurrentNode->Data.PhraseManualTime;
	else if (CurrentNode->Data.Sound != NULL)
		return CurrentNode->Data.Sound->Duration;
	else
	{
		if (CurrentNode->Data.Source == EDialogPhraseSource::Player)
			return 0.0f;

		int len = CurrentNode->Data.Text.ToString().Len();
		return .2f + len * 0.05f;
	}
}

bool UDialogImplementer::GetValidPhrases(TArray<UDialogPhrase*>& result, TArray<UDialogNode*> Phrases, bool& IsPlayer)
{
	result.Reset();
	bool found = false;
	IsPlayer = false;

	for (auto& node : Phrases)
	{
		auto phrase = Cast<UDialogPhrase>(node);
		if (phrase && phrase->Check(this))
		{
			if (!found)
			{
				IsPlayer = phrase->Data.Source == EDialogPhraseSource::Player;
			}
			else if (IsPlayer != (phrase->Data.Source == EDialogPhraseSource::Player))
			{
				UE_LOG(DialogModuleLog, Error, TEXT("Invalid graph: Phrase cannot simultaneously refer to phrases of different types"));
				return false;
			}

			result.Add(phrase);
			found = true;
		}
	}

	return found;
}

void UDialogImplementer::OnNextTimer()
{
	Next(0);
}