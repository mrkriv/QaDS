#pragma once

#include "QuestStage.h"
#include "QuestNodes.generated.h"

class UQuestAsset;	
class UQuestProcessor;

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestNode : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UQuestAsset* OwnerQuest;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> Childs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestStageInfo Stage;

	virtual bool Ckeck(UQuestProcessor* processor);
	virtual void InvokePostScript(UQuestProcessor* processor);
	virtual void Assign(UQuestProcessor* processor);
	virtual TArray<UQuestNode*> FindNextStage();
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UQuestEndNode : public UQuestNode
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool IsSuccesEnd;

	virtual bool Ckeck(UQuestProcessor* processor) override;
	virtual void InvokePostScript(UQuestProcessor* processor) override;
	virtual void Assign(UQuestProcessor* processor) override;
};