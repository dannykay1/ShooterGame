// Copyright Danny Kay 2020.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShooterGameTarget : TargetRules
{
	public ShooterGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "ShooterGame" } );
	}
}
