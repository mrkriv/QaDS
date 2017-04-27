#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "DialogPhrase.generated.h"

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
	FString FindTag;

	UPROPERTY()
	class UDialogNode* OwnerNode;

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
class DIALOGSYSTEMRUNTIME_API UDialogNode : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<UDialogNode*> Childs;

	UPROPERTY()
	class UDialogAsset* OwnerDialog;

	virtual void Invoke(class UDialogImplementer* Implementer);
	virtual bool Check(class UDialogImplementer* Implementer) const;

	void AddEvent(TArray<FDialogPhraseEvent>* Array, const FDialogPhraseEvent& Event);
	void AddCondition(TArray<FDialogPhraseCondition>* Array, const FDialogPhraseCondition& Event);

	TArray<UDialogNode*> GetChilds();
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogPhrase : public UDialogNode
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

	void Invoke(class UDialogImplementer* Implementer) override;
	bool Check(class UDialogImplementer* Implementer) const override;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogWait : public UDialogNode
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FText Description;

	UPROPERTY(BlueprintReadOnly)
	float MinTime;

	UPROPERTY(BlueprintReadOnly)
	float MaxTime;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> WaitGiveKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> WaitRemoveKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> EndGiveKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> EndRemoveKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogPhraseCondition> WaitConditions;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogPhraseEvent> EndEvents;
};
