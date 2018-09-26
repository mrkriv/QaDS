#pragma once

#include "Engine/DataAsset.h"
#include "QuestAsset.generated.h"

class UQuestNode;
class AQuestScript;

UENUM(BlueprintType)
enum class EQuestCompleteStatus : uint8
{
	None,
	Active,
	Failed,
	Completed,
	Skiped
};

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

	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY()
	UQuestNode* RootNode;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> ArchiveNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestNode*> ActiveNodes;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "QuestScript"))
	TAssetSubclassOf<AQuestScript> QuestScriptClass;

	UPROPERTY(BlueprintReadOnly)
	AQuestScript* QuestScript;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif	// WITH_EDITORONLY_DATA
};