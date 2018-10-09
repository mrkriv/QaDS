#pragma once

#include "QuestStageEvent.h"
#include "StoryTriggerManager.h"
#include "QuestNode.generated.h"

UENUM(BlueprintType)
enum class EQuestCompleteStatus : uint8
{
	None,
	Active,
	Failed,
	Completed,
	Skiped
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FStoryTriggerCondition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TriggerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> ParamsMasks;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestStageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Stage")
	FGuid UID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FText Caption;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	bool bIsOptional;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TMap<FName, FString> AditionalData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	bool bGenerateEvents = true;

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
	TArray<FStoryTriggerCondition> WaitTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FQuestStageCondition> WaitPredicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FName> FailedIfGiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FName> FailedIfRemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FStoryTriggerCondition> FailedTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	TArray<FQuestStageCondition> FailedPredicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Failed")
	bool bFailedQuest = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FName> GiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FName> RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FQuestStageEvent> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	EQuestCompleteStatus ChangeQuestState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	EQuestCompleteStatus ChangeOderActiveStagesState;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestRuntimeNode : public UObject
{
	GENERATED_BODY()

	TArray<UQuestRuntimeNode*> childCahe;

	void Activate();
	void Failed();
	void Complete();
	void Deactivate();

	bool CkeckForActivate();
	bool CkeckForComplete();
	bool CkeckForFailed();

	bool MatchTringger(FStoryTriggerCondition& condition, const FStoryTrigger& trigger);
	bool MatchTringgerParam(const FString& value, const FString& filter);

public:
	UPROPERTY()
	class UQuestProcessor* Processor;

	UPROPERTY()
	class UQuestRuntimeAsset* OwnerQuest;

	UPROPERTY()
	TArray<FGuid> Childs;

	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestStageInfo Stage;

	bool TryComplete();
	void SetStatus(EQuestCompleteStatus NewStatus);
	TArray<UQuestRuntimeNode*> GetNextStage();

private:
	UFUNCTION()
	void OnChangeStoryKey(const FName& key);

	UFUNCTION()
	void OnTrigger(const FStoryTrigger& Trigger);
};