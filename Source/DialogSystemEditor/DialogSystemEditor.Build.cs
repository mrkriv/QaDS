using UnrealBuildTool;

public class DialogSystemEditor : ModuleRules
{
    public DialogSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "DialogSystemEditor/Public",
            }
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                "DialogSystemEditor/Private",
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
                "InputCore",
                "UnrealEd",
                "PropertyEditor",
                "LevelEditor",
                "EditorStyle",
                "GraphEditor",
                "BlueprintGraph",
                "Projects",


                "DialogSystemRuntime",
            }
        );
    }
}