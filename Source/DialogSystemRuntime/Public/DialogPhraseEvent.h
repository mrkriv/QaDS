// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "DialogPhraseEvent.generated.h"

/*
	Method of selecting an object for calling functions
*/
UENUM(BlueprintType)
enum class EDialogPhraseEventCallType : uint8
{
	DialogScript,
	Player,
	Interlocutor,
	FindByTag,
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEvent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	TSubclassOf<UObject> ObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	EDialogPhraseEventCallType CallType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	FString FindTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phrase")
	FString Command;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	class UDialogNode* OwnerNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	TArray<FString> Parameters;

	virtual bool Compile(FString& ErrorMessage, bool& needUpdate);
	virtual UObject* GetObject(class UDialogImplementer* Implementer) const;
	virtual void Invoke(class UDialogImplementer* Implementer);
	virtual ~FDialogPhraseEvent() {}
};


USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseCondition : public FDialogPhraseEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool InvertCondition;

	virtual bool InvokeCheck(class UDialogImplementer* Implementer); 
	bool Compile(FString& ErrorMessage, bool& needUpdate) override;
	virtual ~FDialogPhraseCondition() {}

private:
	bool CallCheckFunction(UObject* Executor, const TCHAR* Str, bool& checkResult);
};