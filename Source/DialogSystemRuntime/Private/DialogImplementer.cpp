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

	UDialogImplementer* impl = NewObject<UDialogImplementer>();
	impl->Interlocutor = InInterlocutor;
	impl->Asset = DialogAsset;
	impl->isInit = false;

	if (DialogAsset->RootNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Dialog asset '%s' not have root node"), *DialogAsset->GetPathName());
		return NULL;
	}

	bool isPlayerNext;
	impl->GetValidPhrases(impl->NextNodes, DialogAsset->RootNode->GetChilds(), isPlayerNext);

	if (impl->NextNodes.Num() == 0)
		return NULL;

	if (impl->Asset->DialogScriptClass != NULL)
	{
		impl->Asset->DialogScript = InInterlocutor->GetWorld()->SpawnActor<ADialogScript>(impl->Asset->DialogScriptClass, FTransform());
		
		FScriptDelegate dlg;
		dlg.BindUFunction(impl->Asset->DialogScript, "Destroy");
		impl->OnEndDialog.Add(dlg);
	}

	return impl;
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
			OnEndDialog.Broadcast();
			CurrentNode = NULL;
			return;
		}

		if (PhraseIndex < 0 || PhraseIndex >= NextNodes.Num())
		{
			UE_LOG(DialogModuleLog, Error, TEXT("Invalid phrase index"));
			OnEndDialog.Broadcast();
			return;
		}
	}
	else
		isInit = true;

	CurrentNode = NextNodes[PhraseIndex];
	CurrentNode->Invoke(this);

	bool isPlayerNext;
	GetValidPhrases(NextNodes, CurrentNode->GetChilds(), isPlayerNext);

	FDialogPhraseInfo phraseInfo;
	phraseInfo.Text = CurrentNode->Text;
	phraseInfo.Sound = CurrentNode->Sound;

	if (CurrentNode->IsPlayer)
	{
		if (isPlayerNext)
		{
			UE_LOG(DialogModuleLog, Error, TEXT("Invalid graph: Phrase of the player can not point to another player phrase"));
			OnEndDialog.Broadcast();
			return;
		}

		phraseInfo.Source = EDialogPhraseSource::Player;
		OnActivatePhrase.Broadcast(phraseInfo);

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
				answerInfo.Text = node->Text;
				answerInfo.Important = node->Important;
				answerInfo.Read = 
					node->UID.IsValid() &&
					UStoryKeyManager::HasKey(*(Asset->Name.ToString() + node->UID.ToString()), EStoryKeyTypes::DialogPhrases);

				playerPhrases.Add(answerInfo);
			}
		}
		else
			DelayNext();

		phraseInfo.Source = EDialogPhraseSource::Interlocutor;
		OnActivatePhrase.Broadcast(phraseInfo);
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
	if (!CurrentNode->AutoTime)
		return CurrentNode->PhraseManualTime;
	else if (CurrentNode->Sound != NULL)
		return CurrentNode->Sound->Duration;
	else
	{
		if (CurrentNode->IsPlayer)
			return 0.0f;

		int len = CurrentNode->Text.ToString().Len();
		return .2f * len + len * 0.05f;
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
				IsPlayer = phrase->IsPlayer;
			}
			else if (IsPlayer != phrase->IsPlayer)
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