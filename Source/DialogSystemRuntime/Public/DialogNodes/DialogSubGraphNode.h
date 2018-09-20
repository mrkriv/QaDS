#pragma once

#include "DialogNode.h"
#include "DialogSubGraphNode.generated.h"

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogSubGraphNode : public UDialogNode
{
	GENERATED_BODY()
public:

	UPROPERTY()
	class UDialogAsset* TargetDialog;
	
	virtual void Invoke(UDialogProcessor* processor) override;
	virtual bool Check(UDialogProcessor* processor) override;
	virtual TArray<UDialogPhraseNode*> GetNextPhrases(UDialogProcessor* processor) override;
};