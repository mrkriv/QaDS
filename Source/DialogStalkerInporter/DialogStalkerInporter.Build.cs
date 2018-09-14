using UnrealBuildTool;

public class DialogStalkerInporter : ModuleRules
{
    public DialogStalkerInporter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "DialogStalkerInporter/Public",
            }
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                "DialogStalkerInporter/Private",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "Projects",
                "UnrealEd",
                "WorkspaceMenuStructure",
                "XmlParser",

                "DialogSystemRuntime",
                "DialogSystemEditor",
            }
        );
    }
}