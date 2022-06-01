// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "MinesweeperPluginCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperPluginModule"

void FMinesweeperPluginCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "MinesweeperPlugin", "Bring up MinesweeperPlugin window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE