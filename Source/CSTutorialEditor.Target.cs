using UnrealBuildTool;
using System.Collections.Generic;

public class CSTutorialEditorTarget : TargetRules
{
	public CSTutorialEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("CSTutorial");
	}
}
