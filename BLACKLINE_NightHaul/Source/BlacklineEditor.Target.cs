using UnrealBuildTool;
using System.Collections.Generic;

public class BlacklineEditorTarget : TargetRules
{
	public BlacklineEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest; // VERIFY-5.8
		ExtraModuleNames.Add("Blackline");
	}
}
