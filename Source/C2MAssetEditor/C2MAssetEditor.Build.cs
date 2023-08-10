// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class C2MAssetEditor : ModuleRules
{
	public C2MAssetEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"MainFrame",
//				"WorkspaceMenuStructure",
			});

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
				"C2MAssetEditor/Private/Utility",
				"C2MAsset/Public/Structures",
				"C2MAsset/Private/Structures",
			});
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"ContentBrowser",
				"Core",
				"CoreUObject",
				"DesktopWidgets",
				"EditorStyle",
				"Engine",
				"InputCore",
				"Projects",
				"Slate",
				"SlateCore",
				"C2MAsset",
				"UnrealEd",
				"ApplicationCore",
				"MeshDescription",
				"RawMesh",
				"StaticMeshDescription"
			});

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"UnrealEd",
//				"WorkspaceMenuStructure",
			});
	}
}
