#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestNode.h"
#include "QaDSEdGraphNode.h"
#include "QuestEditorNodes.generated.h"

class UEdGraphPin;
class FXmlNode;

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestEdGraphNode : public UQaDSEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UQuestNode* CompileNode;

	virtual UObject* GetCompile() override { return CompileNode; }
	virtual void ResetCompile() override { CompileNode = NULL; }
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestStageEdGraphNode : public UQuestEdGraphNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuestStageInfo Stage;

	virtual void AllocateDefaultPins() override;
	virtual FXmlWriteNode SaveToXml() const;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById);
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestRootEdGraphNode : public UQuestEdGraphNode
{
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual FXmlWriteNode SaveToXml() const;
	virtual void LoadInXml(FXmlReadNode* reader, const TMap<FString, UQaDSEdGraphNode*>& nodeById);
};