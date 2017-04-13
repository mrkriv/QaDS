#include "DialogSystemRuntime.h"
#include "EngineUtils.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"

UStoryKeyManager::UStoryKeyManager()
{
	//if (UStoryKeyManager::GetInstance() != NULL)
	//	UE_LOG(DialogModuleLog, Warning, TEXT("More than one instance StoryKeyManager was created"));

	PrimaryComponentTick.bCanEverTick = false;
}


void UStoryKeyManager::BeginPlay()
{
	Super::BeginPlay();
}


UStoryKeyManager* UStoryKeyManager::GetInstance()
{
	for (TObjectIterator<UStoryKeyManager> Itr; Itr; ++Itr)
		return *Itr;

	return NULL;
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
	auto Instance = GetInstance();
	if (Instance == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager instance not found"));
		return false;
	}

	return Instance->GetSetByType(Type)->Contains(Key);
}

bool UStoryKeyManager::DontHasKey(FName Key, EStoryKeyTypes Type)
{
	auto Instance = GetInstance();
	if (Instance == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager instance not found"));
		return true;
	}

	return !Instance->GetSetByType(Type)->Contains(Key);
}

bool UStoryKeyManager::AddKey(FName Key, EStoryKeyTypes Type)
{
	auto Instance = GetInstance();
	if (Instance == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager instance not found"));
		return false;
	}

	if (Instance->GetSetByType(Type)->Contains(Key))
		return false;

	Instance->GetSetByType(Type)->Add(Key);
	return true;
}

bool UStoryKeyManager::RemoveKey(FName Key, EStoryKeyTypes Type)
{
	auto Instance = GetInstance();
	if (Instance == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager instance not found"));
		return false;
	}

	return Instance->GetSetByType(Type)->Remove(Key) == 1;
}

void UStoryKeyManager::ClearType(EStoryKeyTypes Type)
{
	auto Instance = GetInstance();
	if (Instance == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager instance not found"));
		return;
	}

	Instance->GetSetByType(Type)->Reset();
}

void UStoryKeyManager::ClearAllTypes()
{
	auto Instance = GetInstance();
	if (Instance == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("StoryKeyManager instance not found"));
		return;
	}

	Instance->Keys.Reset();
	Instance->DialogPhrasesKeys.Reset();
}
