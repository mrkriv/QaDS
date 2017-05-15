// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "DialogAsset.h"
#include "DialogImplementer.generated.h"

class URootNode;
class UPhraseNode;
class UDialogNodeEditorBase;

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogAnswerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText Text;

	UPROPERTY(BlueprintReadOnly)
	bool Important;

	UPROPERTY(BlueprintReadOnly)
	bool Read;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogEndSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogAnswersSignature, const TArray<FDialogAnswerInfo>&, Answers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogPhraseSignature, FDialogPhraseInfo, Phrase);

UCLASS(BlueprintType)
class DIALOGSYSTEMRUNTIME_API UDialogImplementer : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UDialogPhrase* CurrentNode;

	bool isInit;
	FTimerHandle NextTimerHandle;
	TArray<UDialogPhrase*> NextNodes;

	void DelayNext();
	float GetPhraseTime();
	bool GetValidPhrases(TArray<UDialogPhrase*>& result, TArray<UDialogNode*> Phrases, bool& IsPlayer);
	void Init(UDialogAsset* DialogAsset, AActor* InInterlocutor);
	void CallEndDialog();

public:
	UPROPERTY(BlueprintReadOnly)
	UDialogAsset* Asset;

	UPROPERTY(BlueprintReadWrite)
	bool EnableTimer = true;

	UPROPERTY(BlueprintReadWrite)
	AActor* Interlocutor;

	UPROPERTY(BlueprintReadOnly)
	class ADialogScript* DialogScript;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDialogAnswersSignature OnUpdateAnswer;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDialogPhraseSignature OnActivatePhrase;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDialogEndSignature OnEndDialog;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Dialog")
	static UDialogImplementer* ImplementDialog(UDialogAsset* DialogAsset, AActor* InInterlocutor);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Dialog")
	void Next(int PhraseIndex);

	void OnNextTimer();
};