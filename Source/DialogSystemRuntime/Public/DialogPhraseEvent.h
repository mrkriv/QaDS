#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "DialogPhraseEvent.generated.h"

DECLARE_DELEGATE_RetVal(bool, FDialogConditionDelegate);

UENUM(BlueprintType)
enum class EDialogPhraseEventCallType : uint8
{
	DialogScript,
	Player,
	Interlocutor,
	FindByTag,
	CreateNew,
};

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEventParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;

	bool Compile(const UProperty* Property, TArray<uint8>& ParameterData, FString& ErrorMessage) const;
};

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEvent
{
	GENERATED_BODY()

private:
	bool CompileParametrs(UObject* Object, FString& ErrorMessage);

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

	virtual bool Check(FString& ErrorMessage);
	virtual UObject* GetObject(class UDialogImplementer* Implementer) const;
	virtual void Invoke(class UDialogImplementer* Implementer) const;
};

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseCondition : public FDialogPhraseEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool InvertCondition;

	virtual bool Check(FString& ErrorMessage) override;
	virtual bool InvokeCheck(class UDialogImplementer* Implementer) const;
};