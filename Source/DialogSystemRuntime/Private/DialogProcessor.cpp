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
	impl->Asset = DialogAsset;

	return impl;
}

void UDialogProcessor::StartDialog()
{
	StoryKeyManager = UStoryKeyManager::GetStoryKeyManager();

	if (Asset->DialogScriptClass != NULL && NPC != NULL)
	{
		DialogScript = NPC->GetWorld()->SpawnActor<ADialogScript>(Asset->DialogScriptClass, FTransform());
		DialogScript->Implementer = this;
	}

	SetCurrentNode(Asset->RootNode);
}

void UDialogProcessor::SetCurrentNode(UDialogNode* node)
{
	IsPlayerNext = false;
	CurrentNode = node;
	NextNodes.Reset();

	for (auto childNode : CurrentNode->Childs)
	{
		if (!CheckNode(childNode))
			continue;

		NextNodes.Append(GetNextPhraseNode(childNode));

		auto phrase = Cast<UDialogPhraseNode>(childNode);
		if (phrase)
		{
			IsPlayerNext = phrase->Data.Source == EDialogPhraseSource::Player;
		}
	}

	if (node->IsA(UDialogPhraseNode::StaticClass()))
	{
		InvokeNode(Cast<UDialogPhraseNode>(node));
	}
	else if (node->IsA(UDialogSubGraphNode::StaticClass()))
	{
		InvokeNode(Cast<UDialogSubGraphNode>(node));
	}
	else if (node->IsA(UDialogElseIfNode::StaticClass()))
	{
		InvokeNode(Cast<UDialogElseIfNode>(node));
	}
	else
	{
		InvokeNode(node);
	}
}

bool UDialogProcessor::CheckNode(UDialogNode* node)
{
	if (node->IsA(UDialogPhraseNode::StaticClass()))
	{
		auto phraseNode = Cast<UDialogPhraseNode>(node);

		for (auto key : phraseNode->Data.CheckHasKeys)
		{
			if (StoryKeyManager->DontHasKey(key))
				return false;
		}

		for (auto key : phraseNode->Data.CheckDontHasKeys)
		{
			if (StoryKeyManager->HasKey(key))
				return false;
		}

		for (auto& Conditions : phraseNode->Data.Predicate)
		{
			if (!Conditions.InvokeCheck(this))
				return false;
		}
	}

	return true;
}

bool UDialogProcessor::CheckCondition(const FDialogElseIfCondition& condition)
{
	for (auto key : condition.CheckHasKeys)
	{
		if (StoryKeyManager->DontHasKey(key))
			return false;
	}

	for (auto key : condition.CheckDontHasKeys)
	{
		if (StoryKeyManager->HasKey(key))
			return false;
	}

	for (auto& predicate : condition.Predicate)
	{
		if (!predicate.InvokeCheck(this))
			return false;
	}

	return true;
}

void UDialogProcessor::InvokeNode(UDialogNode* node)
{
	for (auto child : node->Childs)
	{
		if (CheckNode(child))
		{
			SetCurrentNode(child);
			return;
		}
	}
}

void UDialogProcessor::InvokeNode(UDialogElseIfNode* node)
{
	for (auto conditions : node->Conditions)
	{
		if (!CheckCondition(conditions))
			continue;

		for (auto nextNode : conditions.NextNode)
		{
			if (CheckNode(nextNode))
			{
				SetCurrentNode(nextNode);
			}
		}
		break;
	}

	EndDialog();
}

void UDialogProcessor::InvokeNode(UDialogSubGraphNode* node)
{
	if (node->TargetDialog == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Sub dialog is empty in dialog %s"), *Asset->GetPathName());
		return;
	}

	if (node->TargetDialog->RootNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Sub dialog %s have not root node"), *node->TargetDialog->GetPathName());
		return;
	}

	Asset = node->TargetDialog;
	if (Asset->DialogScriptClass == NULL)
	{
		DialogScript = NULL;
	}
	else if(DialogScript == NULL || !DialogScript->IsA(Asset->DialogScriptClass))
	{
		DialogScript = NPC->GetWorld()->SpawnActor<ADialogScript>(Asset->DialogScriptClass, FTransform());
		DialogScript->Implementer = this;
	}

	SetCurrentNode(Asset->RootNode);
}

void UDialogProcessor::InvokeNode(UDialogPhraseNode* node)
{
	if (NextTimerHandle.IsValid() && NPC != NULL)
		NPC->GetWorldTimerManager().ClearTimer(NextTimerHandle);
	
	for (auto key : node->Data.GiveKeys)
		StoryKeyManager->AddKey(key);

	for (auto key : node->Data.RemoveKeys)
		StoryKeyManager->RemoveKey(key);

	for (auto& Event : node->Data.Action)
		Event.Invoke(this);

	//StoryKeyManager->AddKey(node->Data.UID);

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

//todo:: use OOP, this is trash
TArray<UDialogPhraseNode*> UDialogProcessor::GetNextPhraseNode(UDialogNode* node)
{
	TArray<UDialogPhraseNode*> result;

	if (node->IsA(UDialogPhraseNode::StaticClass()))
	{
		result.Add(Cast<UDialogPhraseNode>(node));
	}
	else if (node->IsA(UDialogSubGraphNode::StaticClass()))
	{
		auto subGraphNode = Cast<UDialogSubGraphNode>(node);

		if (subGraphNode->TargetDialog != NULL && subGraphNode->TargetDialog->RootNode != NULL)
		{
			result.Append(GetNextPhraseNode(subGraphNode->TargetDialog->RootNode));
		}
	}
	else if (node->IsA(UDialogElseIfNode::StaticClass()))
	{
		auto elseIfNode = Cast<UDialogElseIfNode>(node);

		for (auto cond : elseIfNode->Conditions)
		{
			if (CheckCondition(cond))
			{
				for (auto next : cond.NextNode)
				{
					result.Append(GetNextPhraseNode(next));
				}
			}
		}
		return GetNextPhraseNode(Cast<UDialogElseIfNode>(node));
	}
	else 
	{
		for (auto child : node->Childs)
		{
			if (CheckNode(child))
				result.Append(GetNextPhraseNode(child));
		}
	}

	return result;
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

		/*if (phraseNode->Data.Source == EDialogPhraseSource::Player)
			return 0.0f;

		int len = phraseNode->Data.Text.ToString().Len();
		return .2f + len * 0.05f;*/
	}
}

void UDialogProcessor::EndDialog()
{
	if (DialogScript != NULL)
		DialogScript->Destroy();

	OnEndDialog.Broadcast();
}