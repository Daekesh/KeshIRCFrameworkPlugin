using UnrealBuildTool;

public class KeshIRCFrameworkPlugin : ModuleRules
{
    public KeshIRCFrameworkPlugin(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
        //PrivateDependencyModuleNames.AddRange(new string[] { "" });
        //DynamicallyLoadedModuleNames.AddRange(new string[] { "" });
    }
}
