// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"

UStoryKeyManager* UStoryKeyManager::GetInstance()
{
	for (TObjectIterator<UStoryKeyManager> Itr; Itr; ++Itr)
		return *Itr;

	return NewObject<UStoryKeyManager>();
}

TArray<FName>* UStoryKeyManager::GetSetByType(EStoryKeyTypes Type)
{
	switch (Type)
	{
	case EStoryKeyTypes::General:
		return &Keys;
		break;
	case EStoryKeyTypes::DialogPhrases:
		return &DialogPhrasesKeys;
		break;
	default:
		return &Keys;
		break;
	}
}

bool UStoryKeyManager::HasKey(FName Key, EStoryKeyTypes Type)
{
	auto inst = GetInstance();
	if (inst == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager inst not found"));
		return false;
	}

	return inst->GetSetByType(Type)->Contains(Key);
}

bool UStoryKeyManager::DontHasKey(FName Key, EStoryKeyTypes Type)
{
	auto inst = GetInstance();
	if (inst == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager inst not found"));
		return true;
	}

	return !inst->GetSetByType(Type)->Contains(Key);
}

bool UStoryKeyManager::AddKey(FName Key, EStoryKeyTypes Type)
{
	auto inst = GetInstance();
	if (inst == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager inst not found"));
		return false;
	}

	if (inst->GetSetByType(Type)->Contains(Key))
		return false;

	inst->GetSetByType(Type)->Add(Key);
	return true;
}

bool UStoryKeyManager::RemoveKey(FName Key, EStoryKeyTypes Type)
{
	auto inst = GetInstance();
	if (inst == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager inst not found"));
		return false;
	}

	return inst->GetSetByType(Type)->Remove(Key) == 1;
}

void UStoryKeyManager::ClearType(EStoryKeyTypes Type)
{
	auto inst = GetInstance();
	if (inst == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager inst not found"));
		return;
	}

	inst->GetSetByType(Type)->Reset();
}

void UStoryKeyManager::ClearAllTypes()
{
	auto inst = GetInstance();
	if (inst == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager inst not found"));
		return;
	}

	inst->Keys.Reset();
	inst->DialogPhrasesKeys.Reset();
}
