using UnrealBuildTool;
using System.Collections.Generic;

public class BlacklineTarget : TargetRules
{
	public BlacklineTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest; // VERIFY-5.8
		ExtraModuleNames.Add("Blackline");
	}
}
