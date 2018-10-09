#include "DialogSystemRuntime.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"
#include "QuestScript.h"

void UQuestRuntimeAsset::CreateScript()
{
	if (!Asset->QuestScriptClass.IsNull())
	{
		Script = GetWorld()->SpawnActor<AQuestScript>(Asset->QuestScriptClass.Get());
		Script->Quest = this;
	}
}

void UQuestRuntimeAsset::DestroyScript()
{
	if (Script != NULL && !Script->IsActorBeingDestroyed())
	{
		Script->Destroy();
	}
}

UQuestRuntimeNode* UQuestRuntimeAsset::LoadNode(FGuid uid)
{
	if (!Asset->Nodes.Contains(uid))
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Node %s not found in %s"), *uid.ToString(), *GetFName().ToString());
		return NULL;
	}

	auto runtimeStage = NewObject<UQuestRuntimeNode>();
	runtimeStage->Processor = UQuestProcessor::GetQuestProcessor();
	runtimeStage->OwnerQuest = this;
	runtimeStage->Childs = Asset->Joins[uid].UIDs;
	runtimeStage->Stage = Asset->Nodes[uid];

	return runtimeStage;
}

FArchive& operator<<(FArchive& Ar, FQuestRuntimeAssetArchive& A)
{
	return Ar
		<< A.AssetName
		<< A.ActiveNodes
		<< A.ArchiveNodes
		<< A.Status;
}

FArchive& operator<<(FArchive& Ar, FQuestRuntimeNodeArchive& A)
{
	return Ar
		<< A.UID
		<< A.Status
		<< A.Progress
		<< A.WaitTriggers
		<< A.FailedTriggers;
}

// Load from archive

UQuestRuntimeAsset* FQuestRuntimeAssetArchive::Load()
{
	auto runtimeAsset = NewObject<UQuestRuntimeAsset>();

	runtimeAsset->Asset = TSoftObjectPtr<UQuestAsset>(AssetName).LoadSynchronous();
	runtimeAsset->Status = Status;

	for (auto ar : ActiveNodes)
	{
		ar.Load(runtimeAsset);
	}

	for (auto ar : ArchiveNodes)
	{
		ar.Load(runtimeAsset);
	}

	return runtimeAsset;
}

UQuestRuntimeNode* FQuestRuntimeNodeArchive::Load(UQuestRuntimeAsset* RuntimeAsset)
{
	auto node = RuntimeAsset->LoadNode(UID);
	node->SetStatus(Status);
	//todo:: node->SetProgress 

	for (auto i = 0; i < WaitTriggers.Num(); i++)
	{
		node->Stage.WaitTriggers[i].TotalCount = WaitTriggers[i];
	}

	for (auto i = 0; i < FailedTriggers.Num(); i++)
	{
		node->Stage.FailedTriggers[i].TotalCount = FailedTriggers[i];
	}

	return node;
}

// Save to archive
FQuestRuntimeAssetArchive::FQuestRuntimeAssetArchive(UQuestRuntimeAsset* RuntimeAsset)
{
	AssetName = FSoftObjectPath(RuntimeAsset->Asset).GetAssetPathString();
	Status = RuntimeAsset->Status;

	for (auto node : RuntimeAsset->ActiveNodes)
		ActiveNodes.Add(node);

	for (auto node : RuntimeAsset->ArchiveNodes)
		ArchiveNodes.Add(node);
}

FQuestRuntimeNodeArchive::FQuestRuntimeNodeArchive(UQuestRuntimeNode* RuntimeNode)
{
	UID = RuntimeNode->Stage.UID;
	Status = RuntimeNode->Status;
	//Progress = RuntimeNode->GetProgress();

	for (auto& trigger : RuntimeNode->Stage.WaitTriggers)
	{
		WaitTriggers.Add(trigger.TotalCount);
	}

	for (auto& trigger : RuntimeNode->Stage.FailedTriggers)
	{
		FailedTriggers.Add(trigger.TotalCount);
	}
}