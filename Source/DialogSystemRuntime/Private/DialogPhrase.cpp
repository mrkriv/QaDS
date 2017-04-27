#include "DialogSystemRuntime.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"
#include "DialogImplementer.h"
#include "DialogPhrase.h"

void UDialogNode::Invoke(class UDialogImplementer* Implementer)
{
}

bool UDialogNode::Check(UDialogImplementer* Implementer) const
{
	return false;
}

TArray<UDialogNode*> UDialogNode::GetChilds()
{
	return Childs;
}

void UDialogNode::AddEvent(TArray<FDialogPhraseEvent>* Array, const FDialogPhraseEvent& Event)
{
	auto ev = Event;
	ev.OwnerNode = this;
	Array->Add(ev);
}

void UDialogNode::AddCondition(TArray<FDialogPhraseCondition>* Array, const FDialogPhraseCondition& Event)
{
	auto cond = Event;
	cond.OwnerNode = this;
	Array->Add(cond);
}

void UDialogPhrase::Invoke(UDialogImplementer* Implementer)
{
	for (auto key : GiveKeys)
		UStoryKeyManager::AddKey(key);

	for (auto key : RemoveKeys)
		UStoryKeyManager::RemoveKey(key);
	
	if (UID.IsValid())
		UStoryKeyManager::AddKey(*(Implementer->Asset->Name.ToString() + UID.ToString()), EStoryKeyTypes::DialogPhrases);

	for (auto& Event : CustomEvents)
		Event.Invoke(Implementer);
}

bool UDialogPhrase::Check(UDialogImplementer* Implementer) const
{
	for (auto key : CheckHasKeys)
	{
		if (UStoryKeyManager::DontHasKey(key))
			return false;
	}

	for (auto key : CheckDontHasKeys)
	{
		if (UStoryKeyManager::HasKey(key))
			return false;
	}

	for (auto& Conditions : CustomConditions)
	{
		if (!Conditions.InvokeCheck(Implementer))
			return false;
	}

	return true;
}