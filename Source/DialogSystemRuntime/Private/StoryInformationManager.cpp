// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"

UStoryKeyManager* UStoryKeyManager::GetStoryKeyManager()
{
	for (TObjectIterator<UStoryKeyManager> Itr; Itr;)
		return *Itr;

	return NewObject<UStoryKeyManager>();
}

bool UStoryKeyManager::HasKey(FName Key) const
{
	return Database.Contains(Key);
}

bool UStoryKeyManager::DontHasKey(FName Key) const
{
	return !Database.Contains(Key);
}

bool UStoryKeyManager::AddKey(FName Key)
{
	if (Database.Contains(Key))
		return false;

	Database.Add(Key);
	OnKeyAdd.Broadcast(Key);
	OnKeyAddBP.Broadcast(Key);

	UE_LOG(DialogModuleLog, Log, TEXT("Add key '%s' to storage"), *Key.ToString());
	return true;
}

bool UStoryKeyManager::RemoveKey(FName Key)
{
	if (!Database.Remove(Key))
		return false;

	OnKeyRemove.Broadcast(Key);
	OnKeyRemoveBP.Broadcast(Key);

	UE_LOG(DialogModuleLog, Log, TEXT("Remove key '%s' from storage"), *Key.ToString());
	return true;
}

TArray<FName> UStoryKeyManager::GetKeys() const
{
	return Database.Array();
}

void UStoryKeyManager::SetKeys(const TSet<FName>& keys)
{
	Database = keys;
	OnKeysLoaded.Broadcast(Database.Array());
	OnKeysLoadedBP.Broadcast(Database.Array());

	UE_LOG(DialogModuleLog, Log, TEXT("Load %d keys to storage"), keys.Num());
}

void UStoryKeyManager::Clear()
{
	Database.Reset();
	OnKeysLoaded.Broadcast(Database.Array());
	OnKeysLoadedBP.Broadcast(Database.Array());

	UE_LOG(DialogModuleLog, Log, TEXT("Clear storage"));
}
