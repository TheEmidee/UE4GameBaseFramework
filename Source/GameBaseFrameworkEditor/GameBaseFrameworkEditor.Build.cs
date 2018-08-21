namespace UnrealBuildTool.Rules
{
    public class GameBaseFrameworkEditor : ModuleRules
    {
        public GameBaseFrameworkEditor( ReadOnlyTargetRules Target )
            : base( Target )
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
            bEnforceIWYU = true;

            PublicIncludePathModuleNames.AddRange(
                new string[] 
                {
                    "Engine",
                    "GameBaseFramework"
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Engine",
                    "Core",
                    "CoreUObject",
                    "UnrealEd",
                    "Slate",
                    "SlateCore",
                    "GameBaseFramework"
                }
            );
        }
    }
}