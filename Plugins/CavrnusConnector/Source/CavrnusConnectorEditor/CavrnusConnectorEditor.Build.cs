// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class CavrnusConnectorEditor : ModuleRules
{

    private string ThirdPartyPath
    {
        //Wrapping this here because ModuleDir itself is thirdPartyPath
        get { return Path.Combine(ModuleDirectory, "ThirdParty"); }
    }

    private void AddDefaultIncludePaths()
    {
        // Add all the public directories
        PublicIncludePaths.Add(ModuleDirectory);        

        //Add Public dir for this module
        string PublicDirectory = Path.Combine(ModuleDirectory, "Public");
        if (Directory.Exists(PublicDirectory))
        {
            PublicIncludePaths.Add(PublicDirectory);
        }

        // Add the base private directory for this module
        string PrivateDirectory = Path.Combine(ModuleDirectory, "Private");
        if (Directory.Exists(PrivateDirectory))
        {
            PrivateIncludePaths.Add(PrivateDirectory);
        }
    }

    public CavrnusConnectorEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        bEnableExceptions = true;
#if UE_5_2_OR_LATER
        IWYUSupport = IWYUSupport.Full;
#else
        bEnforceIWYU = true;
#endif
        AddDefaultIncludePaths();

        PublicDependencyModuleNames.AddRange(
        new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "UMGEditor",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "ContentBrowser",
            "ToolMenus",
            "CavrnusConnector",
            "Blutility",
            "LevelEditor",
            "Projects",
						"EditorFramework"
				}
        );

        //Add include path
        string Platform = string.Empty;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            Platform = "Win64";
        }
        else
        {
            throw new Exception(string.Format("Unsupported platform {0}", Target.Platform.ToString()));
        }
    }
}
