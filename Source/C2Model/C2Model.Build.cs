// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class C2Model : ModuleRules
{
	public C2Model(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"MainFrame",
//				"WorkspaceMenuStructure",
			});
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
    				System.IO.Path.GetFullPath(Target.RelativeEnginePath)+ "Plugins/Experimental/Animation/SkeletalMeshModelingTools/Source/"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"MeshDescription",
				"StaticMeshDescription"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		PrivateDependencyModuleNames.AddRange(new string[] { "SkeletalMeshModelingTools" });
		PrivateIncludePathModuleNames.AddRange(new string[] { "SkeletalMeshModelingTools" });
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ContentBrowser",
				"Core",
				"CoreUObject",
				"DesktopWidgets",
				"EditorStyle",
				"AnimationModifiers",
				"Engine",
				"EditorScriptingUtilities",
				"PhysicsUtilities",
				"InputCore",
				"SkeletalMeshModelingTools",
				"Projects",
				"UnrealEd",
				"Slate",
                "AnimationData",
				"MeshBuilder",
				"SlateCore",
				"UnrealEd",
				"ApplicationCore",
				"MeshDescription",
				"StaticMeshDescription", "AnimationBlueprintLibrary"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
