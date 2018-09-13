// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"

UStoryKeyManager* UStoryKeyManager::GetStoryKeyManager()
{
	for (TObjectIterator<UStoryKeyManager> Itr; Itr; ++Itr)
		return *Itr;

	return NewObject<UStoryKeyManager>();
}

UStoryKeyManager::UStoryKeyManager()
{
	Database.Add(EStoryKeyTypes::General, TSet<FName>());
	Database.Add(EStoryKeyTypes::DialogPhrases, TSet<FName>());
	Database.Add(EStoryKeyTypes::Task, TSet<FName>());
}

bool UStoryKeyManager::HasKey(FName Key, EStoryKeyTypes Type)
{
	return Database[Type].Contains(Key);
}

bool UStoryKeyManager::DontHasKey(FName Key, EStoryKeyTypes Type)
{
	return !Database[Type].Contains(Key);
}

bool UStoryKeyManager::AddKey(FName Key, EStoryKeyTypes Type)
{
	if (Database[Type].Contains(Key))
		return false;
	
	Database[Type].Add(Key);
	return true;
}

bool UStoryKeyManager::RemoveKey(FName Key, EStoryKeyTypes Type)
{
	return Database[Type].Remove(Key) == 1;
}

void UStoryKeyManager::ClearType(EStoryKeyTypes Type)
{
	Database[Type].Reset();
}

void UStoryKeyManager::ClearAll()
{
	ClearType(EStoryKeyTypes::General);
	ClearType(EStoryKeyTypes::DialogPhrases);
	ClearType(EStoryKeyTypes::Task);
}
