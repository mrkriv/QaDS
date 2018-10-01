#include "DialogSystemRuntime.h"
#include "DialogJumpNode.h"
#include "DialogProcessor.h"
#include "DialogNodes.h"
#include "DialogAsset.h"

void UDialogJumpNode::Invoke(UDialogProcessor* processor)
{
	check(processor);

	auto tragetNode = processor->Asset->FindPhraseByUID(TargetNodeUID);
	if (tragetNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Failed jump to node UID: %s"), *TargetNodeUID.ToString());
		return;
	}

	processor->SetCurrentNode(tragetNode);
}

bool UDialogJumpNode::Check(UDialogProcessor* processor)
{
	return true;
}

TArray<UDialogPhraseNode*> UDialogJumpNode::GetNextPhrases(UDialogProcessor* processor)
{
	check(processor);
	TArray<UDialogPhraseNode*> result;

	auto tragetNode = processor->Asset->FindPhraseByUID(TargetNodeUID);

	if (tragetNode != NULL)
		result.Add(tragetNode);

    return result;
}
