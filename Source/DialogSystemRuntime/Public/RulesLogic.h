#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "RulesLogic.generated.h"

UCLASS(Blueprintable, HideDropDown)
class DIALOGSYSTEMRUNTIME_API UBoolLogic : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Logic")
	bool NodeBool();
};