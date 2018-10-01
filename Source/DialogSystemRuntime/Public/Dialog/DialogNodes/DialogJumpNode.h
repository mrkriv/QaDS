#pragma once

#include "DialogNode.h"
#include "DialogJumpNode.generated.h"

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogJumpNode : public UDialogNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName TargetNodeUID;
	
	virtual void Invoke(UDialogProcessor* processor) override;
	virtual bool Check(UDialogProcessor* processor) override;
	virtual TArray<UDialogPhraseNode*> GetNextPhrases(UDialogProcessor* processor) override;
};