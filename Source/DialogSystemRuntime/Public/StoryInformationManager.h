// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "StoryInformationManager.generated.h"

UCLASS()
class DIALOGSYSTEMRUNTIME_API UStoryKeyManager : public UObject
{
	GENERATED_BODY()

	TSet<FName> Database;

public:

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	static UStoryKeyManager* GetStoryKeyManager();

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	bool HasKey(FName Key) const;

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	bool DontHasKey(FName Key) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	bool AddKey(FName Key);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	bool RemoveKey(FName Key);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	void Clear();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	TArray<FName> GetKeys() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	void SetKeys(const TSet<FName>& keys);
};
