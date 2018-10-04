#include "DialogSystemRuntime.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"

FQuestRuntimeAssetArchive UQuestRuntimeAsset::Save()
{
	FQuestRuntimeAssetArchive result;

	result.AssetName = FSoftObjectPath(Asset).GetAssetPathString();
	result.Status = Status;

	for (auto node : ActiveNodes)
		result.ActiveNodes.Add(node->Stage.UID);

	for (auto node : ArchiveNodes)
		result.ActiveNodes.Add(node->Stage.UID);

	return result;
}

UQuestRuntimeAsset* FQuestRuntimeAssetArchive::Load()
{
	auto result = NewObject<UQuestRuntimeAsset>();

	result->Asset = TSoftObjectPtr<UQuestAsset>(AssetName).LoadSynchronous();
	result->Status = Status;

	for (auto uid : ActiveNodes)
	{
		auto assetNode = result->Asset->Nodes[uid];
		result->ActiveNodes.Add(assetNode.Load(result));
	}

	for (auto uid : ArchiveNodes)
	{
		auto assetNode = result->Asset->Nodes[uid];
		result->ArchiveNodes.Add(assetNode.Load(result));
	}

	return result;
}

FArchive& operator<<(FArchive& Ar, FQuestRuntimeAssetArchive& A)
{
	return Ar
		<< A.AssetName
		<< A.ActiveNodes
		<< A.ArchiveNodes
		<< A.Status;
}