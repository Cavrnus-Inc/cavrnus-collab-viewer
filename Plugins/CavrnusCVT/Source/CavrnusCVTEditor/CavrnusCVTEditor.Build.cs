// CavrnusCVTEditor.Build.cs

using UnrealBuildTool;

public class CavrnusCVTEditor : ModuleRules
{
	public CavrnusCVTEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		// Set the type of module to Editor
		bEnableExceptions = true;

		// Enable precompiled headers for faster builds
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// List of public dependency modules that this module links against
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd", // Unreal Editor dependency
			"Slate",
			"SlateCore",
			"EditorStyle",
			"UMG",
			"InputCore",
			"LevelEditor",
			"Projects",
			"CavrnusCVT", // Reference to runtime module
			"CavrnusConnector"
		});

		// List of private dependency modules that this module links against
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CinematicCamera",
			"PropertyEditor",
			"AssetTools",
			"AssetRegistry",
			"Kismet",
			"BlueprintGraph"
		});
	}
}