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
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UStoryKeyManager : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> Keys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> DialogPhrasesKeys;

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	static UStoryKeyManager* GetInstance();

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	static bool HasKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintPure, Category = "Gameplay|StoryKey")
	static bool DontHasKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	static bool AddKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	static bool RemoveKey(FName Key, EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	static void ClearType(EStoryKeyTypes Type = EStoryKeyTypes::General);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|StoryKey")
	static void ClearAllTypes();

protected:
	TArray<FName>* GetSetByType(EStoryKeyTypes Type);
};
