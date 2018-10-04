#include "DialogSystemRuntime.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"

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

FArchive& operator<<(FArchive& Ar, FQuestRuntimeAssetArchive& A)
{
	return Ar
		<< A.AssetName
		<< A.ActiveNodes
		<< A.ArchiveNodes
		<< A.Status;
}

UQuestRuntimeNode* FQuestRuntimeNodeArchive::Load(UQuestRuntimeAsset* RuntimeAsset)
{
	auto node = RuntimeAsset->Asset->Nodes[UID].Load(RuntimeAsset);

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

FArchive& operator<<(FArchive& Ar, FQuestRuntimeNodeArchive& A)
{
	return Ar
		<< A.UID
		<< A.Status
		<< A.Progress
		<< A.WaitTriggers
		<< A.FailedTriggers;
}