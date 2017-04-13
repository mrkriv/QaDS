#include "DialogSystemEditor.h"
#include "DialogCommands.h"

#define LOCTEXT_NAMESPACE "Commands_DialogGraph"

void FDialogCommands::RegisterCommands()
{
	UI_COMMAND(Compile, "Compile", "Compile this dialog graph", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE