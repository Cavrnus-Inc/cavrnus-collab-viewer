// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class CavrnusCVT : ModuleRules
{
	public CavrnusCVT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Json", "JsonUtilities", "Slate", "SlateCore", "zlib", "DeveloperSettings"});

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Archive"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Archive/ZipUtilities"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Archive/ZipUtilities/ThirdParty"));
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}