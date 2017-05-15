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

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEvent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> ObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogPhraseEventCallType CallType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FindTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Command;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UDialogNode* OwnerNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> Parameters;

	virtual bool Compile(FString& ErrorMessage, bool& needUpdate);
	virtual UObject* GetObject(class UDialogImplementer* Implementer) const;
	virtual void Invoke(class UDialogImplementer* Implementer);
};


USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseCondition : public FDialogPhraseEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool InvertCondition;

	virtual bool InvokeCheck(class UDialogImplementer* Implementer); 
	bool Compile(FString& ErrorMessage, bool& needUpdate) override;

private:
	bool CallCheckFunction(UObject* Executor, const TCHAR* Str, bool& checkResult);
};