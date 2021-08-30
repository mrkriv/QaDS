#include "DialogAsset.h"
#include "DialogNodes.h"

UDialogPhraseNode* UDialogAsset::FindPhraseByUID(const FName& UID) const
{
	TArray<UDialogNode*> visitList;
	return FindPhraseByUID(UID, RootNode, visitList);
}

UDialogPhraseNode* UDialogAsset::FindPhraseByUID(const FName& UID, UDialogNode* root, TArray<UDialogNode*> visitList) const
{
	if (visitList.Contains(root))
		return NULL;

	visitList.Add(root);

	auto phraseNode = Cast<UDialogPhraseNode>(root);

	if (phraseNode != NULL && phraseNode->Data.UID == UID)
	{
		return phraseNode;
	}

	for (auto child : root->Childs)
	{
		auto result = FindPhraseByUID(UID, child, visitList);
		if (result != NULL)
			return result;
	}

	return NULL;
}