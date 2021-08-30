#include "DialogNode.h"
#include "DialogProcessor.h"
#include "DialogNodes.h"

void UDialogNode::Invoke(UDialogProcessor* processor)
{
	check(processor);

	for (auto child : Childs)
	{
		if (child->Check(processor))
		{
			processor->SetCurrentNode(child);
			return;
		}
	}
}

bool UDialogNode::Check(UDialogProcessor* processor)
{
	return true;
}

TArray<UDialogPhraseNode*> UDialogNode::GetNextPhrases(UDialogProcessor* processor)
{
	check(processor);

	TArray<UDialogPhraseNode*> result;

    for (auto child : Childs)
    {
		if (child->Check(processor))
            result.Append(child->GetNextPhrases(processor));
    }

    return result;
}
