// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MinesweeperProjectTarget : TargetRules
{
	public MinesweeperProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "MinesweeperProject" } );
	}
}