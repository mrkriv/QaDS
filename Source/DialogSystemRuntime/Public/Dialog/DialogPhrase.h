#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "DialogPhraseEvent.h"
#include "DialogPhrase.generated.h"

class UDialogNode;
class UDialogPhraseNode;
class UDialogSubGraphNode;
class UDialogElseIfNode;
class ADialogScript;
class UDialogAsset;
class UQuestAsset;

UENUM(BlueprintType)
enum class EDialogPhraseSource : uint8
{
	Player,
	NPC,
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseShortInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText Text;

	UPROPERTY(BlueprintReadOnly)
	FName UID;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FString> AditionalData;
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FDialogPhraseInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	FName UID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	bool AutoTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	float PhraseManualTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	TAssetPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	EDialogPhraseSource Source;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phrase")
	TMap<FName, FString> AditionalData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FDialogPhraseCondition> Predicate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FName> GiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FName> RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FDialogPhraseEvent> Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TSoftObjectPtr<UQuestAsset> StartQuest;
};