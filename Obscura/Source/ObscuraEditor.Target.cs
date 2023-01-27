using UnrealBuildTool;

public class ObscuraEditorTarget : TargetRules
{
	public ObscuraEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("ObscuraCore");
	}
}
