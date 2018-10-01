#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "StoryTriggerManager.h"
#include "StrotyVolume.generated.h"

UCLASS()
class DIALOGSYSTEMRUNTIME_API AStrotyVolume : public APhysicsVolume
{
	GENERATED_BODY()

	virtual void ActorEnteredVolume(class AActor* Other) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	TArray<FName> CheckDontHasKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FName> GiveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FName> RemoveKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TArray<FStoryTrigger> ActivateTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	TSoftObjectPtr<class UQuestAsset> StartQuest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activate")
	bool bDestroySelf = true;

	UFUNCTION(BlueprintCallable)
	virtual bool CanActivate(class AActor* Other);

	UFUNCTION(BlueprintCallable)
	virtual void Activate();
};
