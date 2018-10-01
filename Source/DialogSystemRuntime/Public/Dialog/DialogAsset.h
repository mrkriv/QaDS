#pragma once

#include "Engine/DataAsset.h"
#include "DialogAsset.generated.h"

class UDialogPhraseNode;

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UDialogAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY()
	class UDialogNode* RootNode;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "DialogScript"))
	TAssetSubclassOf<class ADialogScript> DialogScriptClass;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif	// WITH_EDITORONLY_DATA

	UFUNCTION(BlueprintCallable)
	UDialogPhraseNode* FindPhraseByUID(const FName& UID) const;

private:
	UDialogPhraseNode* FindPhraseByUID(const FName& UID, UDialogNode* root, TArray<UDialogNode*> visitList) const;
};