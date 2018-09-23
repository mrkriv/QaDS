#pragma once

#include "EngineUtils.h"
#include "Components/ActorComponent.h"
#include "QuestProcessor.generated.h"

class UQuestAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(		FQuestStart,	UQuestAsset*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStartBP,	UQuestAsset*,	Quest);

DECLARE_MULTICAST_DELEGATE_TwoParams(		 FQuestStageChange,		UQuestAsset*,			UQuestNode*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStageChangeBP,	UQuestAsset*,	Quest,	UQuestNode*, Stage);

DECLARE_MULTICAST_DELEGATE_TwoParams(		 FQuestEnd,		UQuestAsset*,			bool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestEndBP,	UQuestAsset*,	Quest,	bool, IsSucces);

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestProcessor : public UObject
{
	GENERATED_BODY()

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

	UFUNCTION(BlueprintPure, Category = "Gameplay|Quest")
	static UQuestProcessor* GetQuestProcessor();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void StartQuest(TAssetPtr<UQuestAsset> QuestAsset);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestAsset* Quest, bool IsSuccses);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestAsset*> GetActiveQuests() const;
};
