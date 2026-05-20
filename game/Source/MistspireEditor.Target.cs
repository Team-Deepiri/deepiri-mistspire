using UnrealBuildTool;

public class MistspireEditorTarget : TargetRules
{
	public MistspireEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("Mistspire");
	}
}
