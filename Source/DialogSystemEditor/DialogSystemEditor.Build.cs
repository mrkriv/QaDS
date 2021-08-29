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
                "TargetPlatform",
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
                "ToolMenus",
                "LevelEditor",
                "EditorStyle",
                "GraphEditor",
                "BlueprintGraph",
                "Projects",
                "ApplicationCore",
                "DesktopPlatform",
                "XmlParser",

                "DialogSystemRuntime",
            }
        );
    }
}