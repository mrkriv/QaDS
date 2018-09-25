#pragma once

#include "DialogPhraseEvent.h" //todo:: create FQuestStageCondition (need update rate param)
#include "QuestNode.generated.h"

class UQuestAsset;	
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
	TArray<FDialogPhraseCondition> Predicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FName> WaitHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FName> WaitDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FDialogPhraseCondition> WaitPredicate; //todo:: create FQuestStageCondition (need update rate param)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FName> GiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FName> RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Complete")
	TArray<FDialogPhraseEvent> Action;
};

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UQuestAsset* OwnerQuest;

	UPROPERTY()
	TArray<UQuestNode*> Childs;

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

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestEndNode : public UQuestNode
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool IsSuccesEnd;
};