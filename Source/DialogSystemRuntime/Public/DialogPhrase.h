// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "DialogPhraseEvent.h"
#include "DialogPhrase.generated.h"

UENUM(BlueprintType)
enum class EDialogPhraseSource : uint8
{
	Player,
	NPC,
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseShortInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText Text;

	UPROPERTY(BlueprintReadOnly)
	FName UID;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FString> AditionalData;
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	FName UID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	bool AutoTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	float PhraseManualTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	EDialogPhraseSource Source;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	TMap<FName, FString> AditionalData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FDialogPhraseCondition> CustomConditions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FName> GiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FName> RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FDialogPhraseEvent> CustomEvents;
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

	virtual void Invoke(class UDialogProcessor* Implementer);
	virtual bool Check(class UDialogProcessor* Implementer);
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogPhrase : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	FDialogPhraseInfo Data;

	void Invoke(class UDialogProcessor* Implementer) override;
	bool Check(class UDialogProcessor* Implementer) override;
};
