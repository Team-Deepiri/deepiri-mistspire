using UnrealBuildTool;

public class Mistspire : ModuleRules
{
	public Mistspire(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"HeadMountedDisplay", "EnhancedInput", "CableComponent", "MistspireOpenXRNative",
			"OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemSteam",
			"PhysicsCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RenderCore",
			"XRBase",
		});
	}
}
