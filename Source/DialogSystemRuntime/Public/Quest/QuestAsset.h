#pragma once

#include "Engine/DataAsset.h"
#include "QuestAsset.generated.h"

class UQuestNode;
class UQuestRuntimeNode;
class AQuestScript;
class UQuestRuntimeAsset;
class UQuestProcessor;

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
	
	UPROPERTY()
	UQuestNode* RootNode;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "QuestScript"))
	TAssetSubclassOf<AQuestScript> QuestScriptClass;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif	// WITH_EDITORONLY_DATA

	UQuestRuntimeAsset* Load(UQuestProcessor* processor);
};

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestRuntimeAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY()
	UQuestRuntimeNode* RootNode;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestRuntimeNode*> ArchiveNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestRuntimeNode*> ActiveNodes;

	UPROPERTY(BlueprintReadOnly)
	AQuestScript* QuestScript;

	UPROPERTY(BlueprintReadOnly)
	UQuestAsset* Asset;
};