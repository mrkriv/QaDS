#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "QuestNode.h"
#include "QuestProcessor.generated.h"

class UQuestAsset;
class UQuestRuntimeAsset;
class UStoryKeyManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartSignature, UQuestRuntimeAsset*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStageCompleteSignature, UQuestRuntimeAsset*, Quest, FQuestStageInfo, Stage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestEndSignature, UQuestRuntimeAsset*, Quest, EQuestCompleteStatus, QuestStatus);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestProcessor : public UObject
{
	GENERATED_BODY()

	static UQuestProcessor* Instance;

	TArray<UQuestRuntimeAsset*> archiveQuests;
	TArray<UQuestRuntimeAsset*> activeQuests;
	bool bIsResetBegin;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestStartSignature OnQuestStart;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestStageCompleteSignature OnStageComplete;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestEndSignature OnQuestEnd;

	UPROPERTY(BlueprintReadOnly)
	UStoryKeyManager* StoryKeyManager;

	UPROPERTY(BlueprintReadOnly)
	UStoryTriggerManager* StoryTriggerManager;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Quest", meta = (WorldContext = "WorldContextObject"))
	static UQuestProcessor* GetQuestProcessor(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void StartQuest(TAssetPtr<UQuestAsset> QuestAsset);

	void CompleteStage(UQuestRuntimeNode* Stage);
	void WaitStage(UQuestRuntimeNode* Stage);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestRuntimeAsset* Quest, EQuestCompleteStatus QuestStatus);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestRuntimeAsset*> GetQuests(EQuestCompleteStatus FilterStatus) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<uint8> SaveToBinary();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void LoadFromBinary(const TArray<uint8>& Data);

	virtual void BeginDestroy() override;

	friend FArchive& operator<<(FArchive& Ar, UQuestProcessor& A);
};
