#pragma once

#include "EdGraph/EdGraphNode.h"
#include "DialogPhrase.h"
#include "DialogEditorNodes.generated.h"

struct FPinDataTypes
{
	static const FString PinType_Root;
};

class DIALOGSYSTEMEDITOR_API FNodePropertyObserver
{
public:
	virtual void OnPropertyChanged(class UEdGraphNode* Sender, const FName& PropertyName) = 0;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API UDialogNodeEditorBase : public UEdGraphNode
{
	GENERATED_BODY()
public:

	virtual TArray<UDialogNodeEditorBase*> GetChildNodes();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;

	TSharedPtr<FNodePropertyObserver> PropertyObserver;
};


UCLASS()
class DIALOGSYSTEMEDITOR_API UPhraseNode : public UDialogNodeEditorBase
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY()
	UDialogPhrase* CompilePhrase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AutoTime = true;

	UPROPERTY(BlueprintReadOnly)
	FGuid UID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhraseManualTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Important = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool MemorizeReading = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Player Phrase?"))
	bool IsPlayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Conditions", meta = (DisplayName = "Conditions"))
	TArray<FDialogPhraseCondition> CustomConditions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Conditions", meta = (DisplayName = "Has Keys"))
	TArray<FName> CheckHasKeys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Conditions", meta = (DisplayName = "Dont Has Keys"))
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Activate", meta = (DisplayName = "Give Keys"))
	TArray<FName> GiveKeys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Activate", meta = (DisplayName = "Remove Keys"))
	TArray<FName> RemoveKeys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Activate", meta = (DisplayName = "Events"))
	TArray<FDialogPhraseEvent> CustomEvents;

	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
};

UCLASS()
class DIALOGSYSTEMEDITOR_API URootNode : public UPhraseNode
{
	GENERATED_UCLASS_BODY()
public:

	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};