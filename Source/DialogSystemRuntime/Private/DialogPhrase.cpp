// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"
#include "DialogProcessor.h"
#include "DialogPhrase.h"

void UDialogNode::Invoke(class UDialogProcessor* Implementer)
{
}

bool UDialogNode::Check(UDialogProcessor* Implementer)
{
	return false;
}

TArray<UDialogNode*> UDialogNode::GetChilds()
{
	return Childs;
}

void UDialogPhrase::Invoke(UDialogProcessor* Implementer)
{
	auto storyKeyManager = UStoryKeyManager::GetStoryKeyManager();

	for (auto key : Data.GiveKeys)
		storyKeyManager->AddKey(key);

	for (auto key : Data.RemoveKeys)
		storyKeyManager->RemoveKey(key);
	
	storyKeyManager->AddKey(Data.UID, EStoryKeyTypes::DialogPhrases);

	for (auto& Event : Data.CustomEvents)
		Event.Invoke(Implementer);
}

bool UDialogPhrase::Check(UDialogProcessor* Implementer)
{
	auto storyKeyManager = UStoryKeyManager::GetStoryKeyManager();

	for (auto key : Data.CheckHasKeys)
	{
		if (storyKeyManager->DontHasKey(key))
			return false;
	}

	for (auto key : Data.CheckDontHasKeys)
	{
		if (storyKeyManager->HasKey(key))
			return false;
	}

	for (auto& Conditions : Data.CustomConditions)
	{
		if (!Conditions.InvokeCheck(Implementer))
			return false;
	}

	return true;
}