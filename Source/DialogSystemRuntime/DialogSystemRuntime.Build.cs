using UnrealBuildTool;

public class DialogSystemRuntime : ModuleRules
{
    public DialogSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
    {

        PublicIncludePaths.AddRange(
            new string[] {
                "DialogSystemRuntime/Public"
            }
        );


        PrivateIncludePaths.AddRange(
            new string[] {
                "DialogSystemRuntime/Private"
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
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}