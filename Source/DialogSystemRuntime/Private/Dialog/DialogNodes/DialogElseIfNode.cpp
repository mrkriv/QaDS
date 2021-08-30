#include "DialogElseIfNode.h"
#include "DialogProcessor.h"
#include "DialogNodes.h"
#include "StoryInformationManager.h"

void UDialogElseIfNode::Invoke(UDialogProcessor* processor)
{
	for (auto& cond : Conditions)
	{
        if (!cond.Check(processor))
			continue;

		for (auto nextNode : cond.NextNode)
		{
			if (nextNode->Check(processor))
			{
				processor->SetCurrentNode(nextNode);
			}
		}
		break;
	}

	processor->EndDialog();
}

bool UDialogElseIfNode::Check(UDialogProcessor* processor)
{
	return true;
}

TArray<UDialogPhraseNode*> UDialogElseIfNode::GetNextPhrases(UDialogProcessor* processor)
{
	TArray<UDialogPhraseNode*> result;

    for (auto& cond : Conditions)
    {
        if (!cond.Check(processor))
			continue;

        for (auto next : cond.NextNode)
        {
            result.Append(next->GetNextPhrases(processor));
        }
    }

    return result;
}

bool FDialogElseIfCondition::Check(UDialogProcessor* processor)
{
	for (auto& key : CheckHasKeys)
	{
		if (processor->StoryKeyManager->DontHasKey(key))
			return false;
	}

	for (auto& key : CheckDontHasKeys)
	{
		if (processor->StoryKeyManager->HasKey(key))
			return false;
	}

	for (auto& predicate : Predicate)
	{
		if (!predicate.InvokeCheck(processor))
			return false;
	}

	return true;
}