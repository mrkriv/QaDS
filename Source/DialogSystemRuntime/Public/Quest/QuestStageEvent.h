#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "QuestStageEvent.generated.h"

/*
	Method of selecting an object for calling functions
*/
UENUM(BlueprintType)
enum class EQuestStageEventCallType : uint8
{
	QuestScript,
	Player,
	FindByTag,
};

class UQuestNode;

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestStageEvent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	TSubclassOf<UObject> ObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	EQuestStageEventCallType CallType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	FString FindTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	FString Command;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	class UQuestNode* OwnerNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	TArray<FString> Parameters;

	virtual bool Compile(FString& ErrorMessage, bool& needUpdate);
	virtual UObject* GetObject(UQuestNode* QuestNode) const;
	virtual void Invoke(UQuestNode* QuestNode);
	virtual ~FQuestStageEvent() {}

	virtual FString ToString() const;
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestStageCondition : public FQuestStageEvent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool InvertCondition;

	virtual bool Compile(FString& ErrorMessage, bool& needUpdate) override;
	virtual bool InvokeCheck(UQuestNode* QuestNode) const;
	virtual FString ToString() const override;
	virtual ~FQuestStageCondition() {}

private:
	bool CallCheckFunction(UObject* Executor, const TCHAR* Str, bool& checkResult) const;
};
