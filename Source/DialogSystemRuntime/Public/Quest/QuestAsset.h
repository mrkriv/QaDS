#pragma once

#include "Engine/DataAsset.h"
#include "QuestAsset.generated.h"

class UQuestNode;
class AQuestScript;

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY()
	UQuestNode* RootNode;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> CompletedNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> SkipedNodes; //todo:: use this

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> FailedNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> ActiveNodes;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "QuestScript"))
	TAssetSubclassOf<AQuestScript> QuestScriptClass;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif	// WITH_EDITORONLY_DATA
};