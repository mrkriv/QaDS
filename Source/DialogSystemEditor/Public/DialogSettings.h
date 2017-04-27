#pragma once

#include "UObject/NoExportTypes.h"
#include "DialogSettings.generated.h"


UCLASS(config = Engine, defaultconfig)
class DIALOGSYSTEMEDITOR_API UDialogSettings : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(config, EditAnywhere, Category = Settings)
	bool AutoCompile = true;

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeButtonDefault = FLinearColor(0.9f, 0.9f, 0.9f);

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeButtonHovered = FLinearColor(0.02f, 0.02f, 0.02f);

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeRoot = FLinearColor(0.0f, 0.4f, 1.0f);

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodePlayer = FLinearColor(1.0f, 0.4f, 0.0f);

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	FLinearColor NodeNPC = FLinearColor(0.08f, 0.08f, 0.08f);

	UPROPERTY(config, EditAnywhere, Category = "Settings|Editor Style")
	int FontSize = 14;
};