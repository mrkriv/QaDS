#pragma once

#include "QuestStageEvent.h"
#include "QuestAsset.h"
#include "QuestNode.generated.h"

class UQuestProcessor;

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestStageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FName SystemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FText Caption;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	bool WaitAllOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TMap<FName, FString> AditionalData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FQuestStageCondition> Predicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FName> WaitHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FName> WaitDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FQuestStageCondition> WaitPredicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FName> FailedIfGiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FName> FailedIfRemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FQuestStageCondition> FailedPredicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FName> GiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FName> RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FQuestStageEvent> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	EQuestCompleteStatus ChangeQuestState;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UQuestAsset* OwnerQuest;

	UPROPERTY()
	TArray<UQuestNode*> Childs;

	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestStageInfo Stage;

	void SetOwnerQuest(UQuestAsset* quest);
	bool TryComplete(UQuestProcessor* processor);
	bool CkeckForActivate(UQuestProcessor* processor);
	bool CkeckForComplete(UQuestProcessor* processor);
	void InvokePostScript(UQuestProcessor* processor);
	void Assign(UQuestProcessor* processor);
	TArray<UQuestNode*> GetNextStage(UQuestProcessor* processor);
};