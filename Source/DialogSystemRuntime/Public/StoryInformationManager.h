#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "StoryInformationManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FStoryKeyChangeSignature, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FStoryKeysChangeSignature, const TArray<FName>&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStoryKeyChangeSignatureBP, const FName&, StoreKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStoryKeysChangeSignatureBP, const TArray<FName>&, StoreKeys);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UStoryKeyManager : public UObject
{
	GENERATED_BODY()

	TSet<FName> Database;

public:

	FStoryKeyChangeSignature OnKeyAdd;
	FStoryKeyChangeSignature OnKeyRemove;
	FStoryKeysChangeSignature OnKeysLoaded;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|StoryKey")
	FStoryKeyChangeSignatureBP OnKeyAddBP;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|StoryKey")
	FStoryKeyChangeSignatureBP OnKeyRemoveBP;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|StoryKey")
	FStoryKeysChangeSignatureBP OnKeysLoadedBP;

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
