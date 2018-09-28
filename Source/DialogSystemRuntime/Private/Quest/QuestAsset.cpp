#include "DialogSystemRuntime.h"
#include "QuestProcessor.h"
#include "QuestAsset.h"

UQuestRuntimeAsset* UQuestAsset::Load(UQuestProcessor* processor)
{
	check(processor);
	check(RootNode);

	auto runtime = NewObject<UQuestRuntimeAsset>();

	runtime->RootNode = RootNode->Load(processor, runtime);
	// todo:: create runtime->QuestScript
	runtime->Asset = this;

	return runtime;
}