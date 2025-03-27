// Copyright (c) 2024 Cavrnus. All rights reserved.
using UnrealBuildTool;
using System.Collections.Generic;

public class CavrnusCollabViewerTarget  : TargetRules
{
	public CavrnusCollabViewerTarget (TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		ExtraModuleNames.AddRange( new string[] { "CavrnusCollabViewer" } );
	}
}