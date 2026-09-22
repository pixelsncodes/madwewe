using UnrealBuildTool;

public class MadweweEditor : ModuleRules
{
    public MadweweEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // Keep the monitor and its editor automation test out of runtime builds.
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "MadweweRuntime", "Slate", "SlateCore", "InputCore", "WorkspaceMenuStructure" });
    }
}
