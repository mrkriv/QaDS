// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "StoryInformationManager.generated.h"

UENUM(BlueprintType)
enum class EStoryKeyTypes : uint8
{
	General,
	DialogPhrases,
	Task
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UStoryKeyManager : public UObject
{
	GENERATED_BODY()

	TMap<EStoryKeyTypes, TSet<FName>> Database;

public:
    UStoryKeyManager();

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	static UStoryKeyManager* GetStoryKeyManager();

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	bool HasKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	bool DontHasKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	bool AddKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	bool RemoveKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	void ClearType(EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	void ClearAll();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	TArray<FName> GetKeys(EStoryKeyTypes Type) const;

	TMap<EStoryKeyTypes, TSet<FName>> GetKeys() const;
};
