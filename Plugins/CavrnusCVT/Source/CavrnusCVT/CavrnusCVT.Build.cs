// Copyright (c) 2024 Cavrnus. All rights reserved.

using System.IO;
using UnrealBuildTool;

public class CavrnusCVT : ModuleRules
{
	public CavrnusCVT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CavrnusConnector", "CoreUObject", "Engine", "InputCore", "Json", "JsonUtilities", "Slate", "SlateCore", "zlib", "DeveloperSettings","DatasmithRuntime"});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"CavrnusConnector",
				"Engine",
				"Slate",
				"SlateCore",
                "XmlParser"
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}