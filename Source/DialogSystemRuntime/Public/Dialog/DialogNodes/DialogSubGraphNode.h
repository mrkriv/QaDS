#pragma once

#include "DialogNode.h"
#include "DialogSubGraphNode.generated.h"

class UDialogAsset;

UCLASS()
class DIALOGSYSTEMRUNTIME_API UDialogSubGraphNode : public UDialogNode
{
	GENERATED_BODY()

	UPROPERTY()
	class UDialogAsset* TargetDialog;
public:
	
	UPROPERTY()
	TAssetPtr<UDialogAsset> TargetDialogAsset;
	
	virtual void Invoke(UDialogProcessor* processor) override;
	virtual bool Check(UDialogProcessor* processor) override;
	virtual TArray<UDialogPhraseNode*> GetNextPhrases(UDialogProcessor* processor) override;
};