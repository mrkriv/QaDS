#include "DialogSystemRuntime.h"
#include "DialogSubGraphNode.h"
#include "DialogProcessor.h"
#include "DialogNodes.h"
#include "DialogAsset.h"

void UDialogSubGraphNode::Invoke(UDialogProcessor* processor)
{
	if (TargetDialog == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Sub dialog is empty in dialog %s"), *processor->Asset->GetPathName());
		return;
	}

	if (TargetDialog->RootNode == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Sub dialog %s have not root node"), *TargetDialog->GetPathName());
		return;
	}

	processor->SetDialogAsset(TargetDialog);
	processor->SetCurrentNode(TargetDialog->RootNode);
}

bool UDialogSubGraphNode::Check(UDialogProcessor* processor)
{
	return true;
}

TArray<UDialogPhraseNode*> UDialogSubGraphNode::GetNextPhrases(UDialogProcessor* processor)
{
	TArray<UDialogPhraseNode*> result;

	if (TargetDialog != NULL && TargetDialog->RootNode != NULL)
	{
		auto next = TargetDialog->RootNode->GetNextPhrases(processor);
		result.Append(next);
	}

    return result;
}
