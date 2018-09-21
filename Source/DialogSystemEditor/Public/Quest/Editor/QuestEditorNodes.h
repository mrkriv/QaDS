#pragma once

#include "EdGraph/EdGraphNode.h"
#include "QuestNodes.h"
#include "DialogEditorNodes.h"
#include "QuestEditorNodes.generated.h"

class UEdGraphPin;
class FXmlNode;

UCLASS()
class DIALOGSYSTEMEDITOR_API UQuestEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UQuestNode* CompileNode;

	TArray<UQuestEdGraphNode*> GetChildNodes() const;
	virtual int GetOrder() const;
	virtual FString SaveToXml(int tabLevel) const;
	virtual void LoadInXml(FXmlNode* xmlNode, const TMap<FString, UQuestEdGraphNode*>& nodeById);
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	TSharedPtr<FNodePropertyObserver> PropertyObserver;
};