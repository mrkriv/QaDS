#pragma once
#include "SlateBasics.h"
#include "Commands.h"
#include "EditorStyle.h"

class FDialogCommands : public TCommands<FDialogCommands>
{
public:

	TSharedPtr<FUICommandInfo> Compile;

	FDialogCommands()
		: TCommands<FDialogCommands>(TEXT("Dialog Graph Commands"), FText::FromString("Dialog Graph Commands"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;
};