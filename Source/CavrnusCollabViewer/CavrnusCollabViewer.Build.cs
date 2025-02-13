// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class CavrnusCollabViewer : ModuleRules
{
	public CavrnusCollabViewer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {"CavrnusConnector", "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore"});
	}
}
