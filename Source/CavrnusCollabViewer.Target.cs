using UnrealBuildTool;
using System.Collections.Generic;

public class CavrnusCollabViewerTarget : TargetRules
{
    public CavrnusCollabViewerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;

#if UE_5_5_OR_LATER
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
#else
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_0;
#endif

        ExtraModuleNames.AddRange(new string[] { "CavrnusCollabViewer" });
    }
}
