#pragma once

#include "Engine/DataAsset.h"
#include "QuestNode.h"
#include "QuestAsset.generated.h"

class UQuestRuntimeNode;
class AQuestScript;
class UQuestRuntimeAsset;
class UQuestProcessor;

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSingltone = true;
	
	UPROPERTY()
	FGuid RootNode;

	UPROPERTY()
	TMap<FGuid, FQuestNode> Nodes;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "QuestScript"))
	TAssetSubclassOf<AQuestScript> QuestScriptClass;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif	// WITH_EDITORONLY_DATA
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestRuntimeAssetArchive
{
	GENERATED_BODY()

	FString AssetName;
	TArray<FGuid> ActiveNodes;
	TArray<FGuid> ArchiveNodes;
	EQuestCompleteStatus Status;

	class UQuestRuntimeAsset* Load();

	friend FArchive& operator<<(FArchive& Ar, FQuestRuntimeAssetArchive& A);
};

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestRuntimeAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestRuntimeNode*> ArchiveNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<UQuestRuntimeNode*> ActiveNodes;

	UPROPERTY(BlueprintReadOnly)
	AQuestScript* QuestScript;

	UPROPERTY(BlueprintReadOnly)
	UQuestAsset* Asset;

	FQuestRuntimeAssetArchive Save();
};