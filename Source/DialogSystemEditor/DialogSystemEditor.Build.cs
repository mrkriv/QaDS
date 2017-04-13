using UnrealBuildTool;

public class DialogSystemEditor : ModuleRules
{
    public DialogSystemEditor(TargetInfo Target)
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
				
				"DialogSystemRuntime",
            }
            );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
            );
    }
}