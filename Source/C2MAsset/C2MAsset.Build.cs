// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class C2MAsset : ModuleRules
{
	public C2MAsset(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"C2MAssetEditor/Private",
				"C2MAssetEditor/Private/AssetTools",
				"C2MAssetEditor/Private/Factories",
				"C2MAssetEditor/Private/Shared",
				"C2MAssetEditor/Private/Styles",
				"C2MAssetEditor/Private/Toolkits",
				"C2MAssetEditor/Private/Widgets",
				"C2MAssetEditor/Private/ImportUI",
				"C2MAsset/Public/Structures"
			});
		PublicIncludePaths.AddRange(
			new string[] {
			});

		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"RawMesh",
				"MeshDescription",
				"StaticMeshDescription"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RawMesh",
				"MeshDescription",
				"StaticMeshDescription"
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
