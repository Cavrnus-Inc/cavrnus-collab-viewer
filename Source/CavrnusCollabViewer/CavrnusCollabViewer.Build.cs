// Copyright (c) 2024 Cavrnus. All rights reserved.

using System.IO;
using UnrealBuildTool;

public class CavrnusCollabViewer : ModuleRules
{
	public CavrnusCollabViewer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {"CavrnusConnector", "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore","DatasmithRuntime"});
	}
}
