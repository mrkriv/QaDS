#pragma once

#include "Engine/DataAsset.h"
#include "QuestNode.h"
#include "QuestAsset.generated.h"

class UQuestRuntimeNode;
class AQuestScript;
class UQuestRuntimeAsset;
class UQuestProcessor;

USTRUCT()
struct DIALOGSYSTEMRUNTIME_API FQuestStageJoin
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGuid> UIDs;
};

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid RootNode;

	UPROPERTY()
	TMap<FGuid, FQuestStageInfo> Nodes;

	UPROPERTY()
	TMap<FGuid, FQuestStageJoin> Joins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSingltone = true;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "QuestScript"))
	TAssetSubclassOf<AQuestScript> QuestScriptClass;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestRuntimeNodeArchive
{
	GENERATED_BODY()

	FGuid UID;
	int Progress;
	EQuestCompleteStatus Status;
	TArray<int> WaitTriggers;
	TArray<int> FailedTriggers;

	FQuestRuntimeNodeArchive() {}
	FQuestRuntimeNodeArchive(class UQuestRuntimeNode* RuntimeNode);

	class UQuestRuntimeNode* Load(class UQuestRuntimeAsset* Asset);
	friend FArchive& operator<<(FArchive& Ar, FQuestRuntimeNodeArchive& A);
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestRuntimeAssetArchive
{
	GENERATED_BODY()

	FString AssetName;
	TArray<FQuestRuntimeNodeArchive> ActiveNodes;
	TArray<FQuestRuntimeNodeArchive> ArchiveNodes;
	EQuestCompleteStatus Status;

	FQuestRuntimeAssetArchive() {}
	FQuestRuntimeAssetArchive(class UQuestRuntimeAsset* RuntimeAsset);

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
	AQuestScript* Script;

	UPROPERTY(BlueprintReadOnly)
	UQuestAsset* Asset;

	class UQuestRuntimeNode* LoadNode(FGuid uid);
	void CreateScript();
	void DestroyScript();
};