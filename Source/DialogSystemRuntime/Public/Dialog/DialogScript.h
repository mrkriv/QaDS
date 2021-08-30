#pragma once

#include "GameFramework/Info.h"
#include "DialogScript.generated.h"

UCLASS(Blueprintable)
class DIALOGSYSTEMRUNTIME_API ADialogScript : public AInfo
{
	GENERATED_BODY()

public:	
	ADialogScript();

	UPROPERTY()
	class UDialogProcessor* Implementer;

	UFUNCTION(BlueprintPure, Category = "Dialog")
	AActor* GetPlayer();

	UFUNCTION(BlueprintPure, Category = "Dialog")
	AActor* GetNPC();
};
