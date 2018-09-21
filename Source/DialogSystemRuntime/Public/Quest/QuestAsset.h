#pragma once

#include "Engine/DataAsset.h"
#include "QuestAsset.generated.h"

class UQuestNode;
class AQuestScript;

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API UQuestAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY()
	UQuestNode* RootNode;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "QuestScript"))
	TSubclassOf<AQuestScript> QuestScriptClass;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* UpdateGraph;
#endif	// WITH_EDITORONLY_DATA
};