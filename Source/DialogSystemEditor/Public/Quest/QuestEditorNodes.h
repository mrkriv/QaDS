#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestNodes.h"
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

	UPROPERTY()
	FQuestStageInfo Stage;

	virtual void AllocateDefaultPins() override;
	virtual FString SaveToXml(int tabLevel) const;
	virtual void LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById);
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestEndEdGraphNode : public UQuestEdGraphNode
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool IsSuccesEnd;

	virtual void AllocateDefaultPins() override;
	virtual FString SaveToXml(int tabLevel) const;
	virtual void LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById);
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestRootEdGraphNode : public UQuestEdGraphNode
{
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual FString SaveToXml(int tabLevel) const;
	virtual void LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQaDSEdGraphNode*>& nodeById);
};