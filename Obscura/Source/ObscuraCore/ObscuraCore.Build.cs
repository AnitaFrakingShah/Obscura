using UnrealBuildTool;

public class ObscuraCore : ModuleRules
{
	public ObscuraCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG"});
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}