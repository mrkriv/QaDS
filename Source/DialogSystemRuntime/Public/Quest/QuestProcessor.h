#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "QuestNode.h"
#include "QuestProcessor.generated.h"

class UQuestAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartSignature, UQuestAsset*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStageCompleteSignature, UQuestAsset*, Quest, FQuestStageInfo, Stage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestEndSignature, UQuestAsset*, Quest, EQuestCompleteStatus, QuestStatus);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestProcessor : public UObject
{
	GENERATED_BODY()

	TArray<UQuestAsset*> archiveQuests;
	TArray<UQuestAsset*> activeQuests;

public:
	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestStartSignature OnQuestStart;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestStageCompleteSignature OnStageComplete;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestEndSignature OnQuestEnd;

	UPROPERTY(BlueprintReadOnly)
	UStoryKeyManager* StoryKeyManager;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Quest")
	static UQuestProcessor* GetQuestProcessor();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void StartQuest(TAssetPtr<UQuestAsset> QuestAsset);

	void CompleteStage(UQuestNode* Stage);
	void WaitStage(UQuestNode* Stage);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestAsset* Quest, EQuestCompleteStatus QuestStatus);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestAsset*> GetQuests(EQuestCompleteStatus FilterStatus) const;
};
