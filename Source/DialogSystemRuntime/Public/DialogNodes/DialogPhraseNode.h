#pragma once

#include "DialogNode.h"
#include "DialogPhraseNode.generated.h"

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogPhraseNode : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	FDialogPhraseInfo Data;
	
	virtual void Invoke(UDialogProcessor* processor) override;
	virtual bool Check(UDialogProcessor* processor) override;
	virtual TArray<UDialogPhraseNode*> GetNextPhrases(UDialogProcessor* processor) override;
};