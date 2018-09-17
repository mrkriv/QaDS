// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "DialogPhrase.h"
#include "DialogNodes.generated.h"

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogNode : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<UDialogNode*> Childs;

	UPROPERTY()
	UDialogAsset* OwnerDialog;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogPhraseNode : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	FDialogPhraseInfo Data;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogSubGraphNode : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY()
	class UDialogAsset* TargetDialog;
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogElseIfNode : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<FDialogElseIfCondition> Conditions;
};
