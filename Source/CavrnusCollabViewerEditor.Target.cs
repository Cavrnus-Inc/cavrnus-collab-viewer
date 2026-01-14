using UnrealBuildTool;
using System.Collections.Generic;

public class CavrnusCollabViewerEditorTarget : TargetRules
{
    public CavrnusCollabViewerEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

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
