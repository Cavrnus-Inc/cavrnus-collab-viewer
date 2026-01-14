// Copyright (c) 2024 Cavrnus. All rights reserved.
using UnrealBuildTool;
using System.Collections.Generic;

public class CavrnusCollabViewerEditorTarget : TargetRules
{
	public CavrnusCollabViewerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		ExtraModuleNames.AddRange( new string[] { "CavrnusCollabViewer" } );
	}
}
