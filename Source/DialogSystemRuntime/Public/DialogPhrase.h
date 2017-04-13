#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "RulesLogic.h"
#include "DialogPhrase.generated.h"

DECLARE_DELEGATE_RetVal(bool, FDialogConditionDelegate);

UENUM(BlueprintType)
enum class EDialogPhraseEventCallType : uint8
{
	Player,
	Interlocutor,
	DialogBlueprint,
	LevelObject,
	FindByTag,
	CreateNew,
};

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> ObjectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogPhraseEventCallType CallType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* LevelObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FindTag;

	virtual bool Check(FString& ErrorMessage) const;
	virtual UObject* GetObject(class UDialogImplementer* Implementer) const;
};

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseCondition : public FDialogPhraseEvent
{
	GENERATED_BODY()

	virtual bool Check(FString& ErrorMessage) const override;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogPhrase : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FText Text;

	UPROPERTY(BlueprintReadOnly)
	bool AutoTime = true;

	UPROPERTY(BlueprintReadOnly)
	bool Important = true;

	UPROPERTY(BlueprintReadOnly)
	float PhraseManualTime;

	UPROPERTY(BlueprintReadOnly)
	FGuid UID;

	UPROPERTY(BlueprintReadOnly)
	USoundBase* Sound;

	UPROPERTY(BlueprintReadOnly)
	bool IsPlayer;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> CheckHasKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> GiveKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> RemoveKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogPhraseCondition> CustomConditions;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogPhraseEvent> CustomEvents;

	UPROPERTY()
	TArray<UDialogPhrase*> Childs;

	virtual void Invoke(class UDialogImplementer* Implementer);
	virtual bool Check(class UDialogImplementer* Implementer) const;

	TArray<UDialogPhrase*> GetChilds();
};
