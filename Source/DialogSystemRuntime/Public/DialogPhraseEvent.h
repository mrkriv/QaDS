// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "DialogPhraseEvent.generated.h"

DECLARE_DELEGATE_RetVal(bool, FDialogConditionDelegate);

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
	CreateNew,
};

/*
	Information about function argument
*/
USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEventParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	FString Type;

	UPROPERTY(EditAnywhere)
	FString Value;

	UPROPERTY(EditAnywhere)
	bool IsOut;

	/*
		Property size
	*/
	UPROPERTY(EditAnywhere)
	int Size;

	/*
		Property offest in argument data
	*/
	UPROPERTY(EditAnywhere)
	int Offest;

	/*
		Compile argument data 
	*/
	bool Compile(const UProperty* Property, TArray<uint8>& ParameterData, FString& ErrorMessage) const;
};

/*
	Information about the function called from the dialog
*/
USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEvent
{
	GENERATED_BODY()

private:
	bool CompileParametrs(UObject* Object, FString& ErrorMessage, bool& isNeedUpdateProp);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> ObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogPhraseEventCallType CallType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FindTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UDialogNode* OwnerNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDialogPhraseEventParam> Parameters;

	UPROPERTY()
	TArray<uint8> ParameterData;

	/*
		Check and compile function call information
	*/
	virtual bool Compile(FString& ErrorMessage, bool& isNeedUpdateProp);

	/*
		Returns an object to call a function
	*/
	virtual UObject* GetObject(class UDialogImplementer* Implementer) const;

	/*
		Calls the function of the object
	*/
	virtual void Invoke(class UDialogImplementer* Implementer) const;
};


USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseCondition : public FDialogPhraseEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool InvertCondition;

	virtual bool Compile(FString& ErrorMessage, bool& isNeedUpdateProp) override;
	virtual bool InvokeCheck(class UDialogImplementer* Implementer) const;
};