using UnrealBuildTool;

public class MadweweEditor : ModuleRules
{
    public MadweweEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "MadweweRuntime" });
    }
}
