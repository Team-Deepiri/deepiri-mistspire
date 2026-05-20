using UnrealBuildTool;
public class MistspireOpenXRNative : ModuleRules
{
	public MistspireOpenXRNative(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "RenderCore", "RHI",
			"HeadMountedDisplay", "OpenXRHMD", "OpenXRCore" });
		PrivateDependencyModuleNames.Add("Projects");
		PrivateIncludePathModuleNames.Add("OpenXRCore");
	}
}
