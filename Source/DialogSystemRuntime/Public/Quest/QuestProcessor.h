#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "QuestNode.h"
#include "QuestProcessor.generated.h"

class UQuestAsset;
class UQuestNode;

DECLARE_MULTICAST_DELEGATE_OneParam(		FQuestStart,	UQuestAsset*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartBP,	UQuestAsset*,	Quest);

DECLARE_MULTICAST_DELEGATE_TwoParams(		 FQuestStageChange,		UQuestAsset*,			FQuestStageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStageChangeBP,	UQuestAsset*,	Quest,	FQuestStageInfo, Stage);

DECLARE_MULTICAST_DELEGATE_TwoParams(		 FQuestEnd,		UQuestAsset*,			bool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestEndBP,	UQuestAsset*,	Quest,	bool, IsSucces);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestProcessor : public UObject
{
	GENERATED_BODY()

	TArray<UQuestAsset*> archiveQuests;
	TArray<UQuestAsset*> activeQuests;

public:
	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestStartBP OnQuestStartBP;
	FQuestStart OnQuestStart;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestStageChangeBP OnQuestStageChangeBP;
	FQuestStageChange OnQuestStageChange;

	UPROPERTY(BlueprintAssignable, Category = "Gameplay|Quest")
	FQuestEndBP OnQuestEndBP;
	FQuestEnd OnQuestEnd;

	UPROPERTY(BlueprintReadOnly)
	UStoryKeyManager* StoryKeyManager;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Quest")
	static UQuestProcessor* GetQuestProcessor();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void StartQuest(TAssetPtr<UQuestAsset> QuestAsset);

	void ActivateStage(UQuestNode* Stage);
	void WaitStage(UQuestNode* Stage);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestAsset* Quest, bool IsSuccses);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestAsset*> GetActiveQuests() const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestAsset*> GetArchiveQuests() const;
};
