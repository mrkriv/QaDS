#pragma once

#include "DialogNode.h"
#include "DialogElseIfNode.generated.h"

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogElseIfCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FDialogPhraseCondition> Predicate;

	UPROPERTY()
	TArray<UDialogNode*> NextNode;

	bool Check(UDialogProcessor* processor);
};

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogElseIfNode : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<FDialogElseIfCondition> Conditions;

	virtual void Invoke(UDialogProcessor* processor) override;
	virtual bool Check(UDialogProcessor* processor) override;
	virtual TArray<UDialogPhraseNode*> GetNextPhrases(UDialogProcessor* processor) override;
};
