// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "Engine/EngineTypes.h"
#include "DialogPhrase.h"
#include "UObject/NoExportTypes.h"
#include "DialogProcessor.generated.h"

class UDialogRootEdGraphNode;
class UDialogPhraseEdGraphNode;
class UDdialogEdGraphNode;
class UDialogNode;
class UDialogPhraseNode;
class UDialogSubGraphNode;
class UDialogElseIfNode;
class ADialogScript;
class UDialogAsset;
class UStoryKeyManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogEndSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangePhraseVariantSignature, const TArray<FDialogPhraseShortInfo>&, Variants);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogPhraseSignature, FDialogPhraseInfo, Phrase);


UCLASS(BlueprintType)
class DIALOGSYSTEMRUNTIME_API UDialogProcessor : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UDialogNode* CurrentNode;

	FTimerHandle NextTimerHandle;
	TArray<UDialogPhraseNode*> NextNodes;
	bool IsPlayerNext;

	void DelayNext();
	float GetPhraseTime();
	void SetCurrentNode(UDialogNode* node);
	void InvokeNode(UDialogNode* node);
	void InvokeNode(UDialogPhraseNode* node);
	void InvokeNode(UDialogSubGraphNode* node);
	void InvokeNode(UDialogElseIfNode* node);
	bool CheckNode(UDialogNode* node);
	bool CheckCondition(const FDialogElseIfCondition& condition);

	void EndDialog();

public:
	UPROPERTY(BlueprintReadOnly)
	UDialogAsset* Asset;

	UPROPERTY(BlueprintReadOnly)
	ADialogScript* DialogScript;

	UPROPERTY(BlueprintReadOnly)
	UStoryKeyManager* StoryKeyManager;

	UPROPERTY(BlueprintReadWrite)
	AActor* NPC;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FChangePhraseVariantSignature OnChangePhraseVariant;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDialogPhraseSignature OnShowPlayerPhrase;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDialogPhraseSignature OnShowNPCPhrase;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDialogEndSignature OnEndDialog;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Dialog")
	static UDialogProcessor* CreateDialogProcessor(UDialogAsset* DialogAsset, AActor* InNPC);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Dialog")
	void StartDialog();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Dialog")
	void Next(FName PhraseUID);

	void OnNextTimer(); 
};