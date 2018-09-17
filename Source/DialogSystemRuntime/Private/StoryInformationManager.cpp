// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"

const FString GetTypeName(EStoryKeyTypes type)
{
	auto EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EStoryKeyTypes"), true);

	if (!EnumPtr)
		return "Invalid";

#if WITH_EDITOR
	return EnumPtr->GetDisplayNameTextByIndex((int32)type).ToString();
#else
	return EnumPtr->GetEnumName((int32)type);
#endif
}

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

	auto typeName = GetTypeName(Type);
	UE_LOG(DialogModuleLog, Log, TEXT("Add key '%s' to storage %s"), *Key.ToString(), *typeName);

	return true;
}

bool UStoryKeyManager::RemoveKey(FName Key, EStoryKeyTypes Type)
{
	if (!Database[Type].Remove(Key))
		return false;

	auto typeName = GetTypeName(Type);
	UE_LOG(DialogModuleLog, Log, TEXT("Remove key '%s' from storage %s"), *Key.ToString(), *typeName);

	return true;
}

TMap<EStoryKeyTypes, TSet<FName>> UStoryKeyManager::GetKeys() const
{
	return Database;
}

TArray<FName> UStoryKeyManager::GetKeys(EStoryKeyTypes Type) const
{
	return Database[Type].Array();
}

void UStoryKeyManager::ClearType(EStoryKeyTypes Type)
{
	auto typeName = GetTypeName(Type);
	UE_LOG(DialogModuleLog, Log, TEXT("Reset storage: '%s'"), *typeName);

	Database[Type].Reset();
}

void UStoryKeyManager::ClearAll()
{
	ClearType(EStoryKeyTypes::General);
	ClearType(EStoryKeyTypes::DialogPhrases);
	ClearType(EStoryKeyTypes::Task);
}
