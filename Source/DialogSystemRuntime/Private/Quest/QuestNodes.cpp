#include "DialogSystemRuntime.h"
#include "QuestNodes.h"
#include "QuestAsset.h"
#include "QuestProcessor.h"

void UQuestNode::InvokePostScript(UQuestProcessor* processor)
{

}

bool UQuestNode::Ckeck(UQuestProcessor* processor)
{
	return true;
}

void UQuestNode::Assign(UQuestProcessor* processor)
{

}

TArray<UQuestNode*> UQuestNode::FindNextStage()
{
	TArray<UQuestNode*> result;

	return result;
}


bool UQuestEndNode::Ckeck(UQuestProcessor* processor)
{
	return true;
}

void UQuestEndNode::InvokePostScript(UQuestProcessor* processor)
{
	Super::InvokePostScript(processor);

	processor->EndQuest(OwnerQuest, IsSuccesEnd);
}

void UQuestEndNode::Assign(UQuestProcessor* processor)
{

}