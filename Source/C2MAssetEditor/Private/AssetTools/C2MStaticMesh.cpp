#include "C2MStaticMesh.h"
#include "C2MMaterialInstance.h"
#include "EditorModeManager.h"
#include "ObjectTools.h"
#include "StaticMeshAttributes.h"
#include "C2MUtilsImport.h"
#include "StaticMeshOperations.h"
#include "EditorFramework/AssetImportData.h"

/* Create FRawMesh from C2Mesh
 * UNUSED
 */
FRawMesh C2MStaticMesh::CreateRawMesh(C2Mesh* InMesh)
{
	FRawMesh RawMesh = FRawMesh();

	int VertexCount = InMesh->Vertices.Num();
	int SurfaceCount = InMesh->Surfaces.Num();
	int FaceOffset = 0;
	RawMesh.FaceMaterialIndices.AddZeroed(InMesh->Header->FaceCount);
	RawMesh.FaceSmoothingMasks.AddZeroed(InMesh->Header->FaceCount);
	
	RawMesh.VertexPositions.AddZeroed(VertexCount);
	for (int i = 0; i < VertexCount; i++)
	{
		RawMesh.VertexPositions[i] = C2MUtilsImport::ConvertDir(InMesh->Vertices[i]);
	}
	
	for (int i = 0; i < SurfaceCount; i++)
	{
		auto Surface = InMesh->Surfaces[i];
		for (auto Face : Surface->Faces)
		{
			for (int faceIndex = 0; faceIndex < 3; faceIndex++)
			{
				uint32_t VertexIndex = Face.index[faceIndex];
				RawMesh.WedgeTangentX.Add(FVector::ZeroVector);
				RawMesh.WedgeTangentY.Add(FVector::ZeroVector);
				RawMesh.WedgeTangentZ.Add(C2MUtilsImport::ConvertDir(InMesh->Normals[VertexIndex]));
				RawMesh.WedgeColors.Add(InMesh->Colors[VertexIndex].ToFColor());
				RawMesh.WedgeIndices.Add(VertexIndex);
				TArray<FVector2D> VertexUVs = InMesh->UVs[i];
				for (int u = 0; u < InMesh->UVSetCount; u++)
					RawMesh.WedgeTexCoords[u].Add(FVector2D(VertexUVs[u].X, 1 - VertexUVs[u].Y)); // We flip V channel of UV

				RawMesh.FaceMaterialIndices[FaceOffset] = i;
				RawMesh.FaceSmoothingMasks[FaceOffset] = 0xFFFFFFFF;
			}
			FaceOffset++;
		}
	}
	
	return RawMesh;
}
/* Create FMeshDescription from C2Mesh
 * 
 */
FMeshDescription C2MStaticMesh::CreateMeshDescription(C2Mesh* InMesh)
{
	// Prepare our base Mesh Description
	FMeshDescription MeshDescription;
	FStaticMeshAttributes CombinedMeshAttributes{ MeshDescription };
	CombinedMeshAttributes.Register();
	CombinedMeshAttributes.RegisterPolygonNormalAndTangentAttributes();
	
	const TVertexAttributesRef<FVector>			  TargetVertexPositions				= CombinedMeshAttributes.GetVertexPositions();
	const TPolygonGroupAttributesRef<FName>		  PolygonGroupNames					= CombinedMeshAttributes.GetPolygonGroupMaterialSlotNames();
	const TVertexInstanceAttributesRef<FVector>   TargetVertexInstanceNormals       = CombinedMeshAttributes.GetVertexInstanceNormals();
	const TVertexInstanceAttributesRef<FVector2D> TargetVertexInstanceUVs           = CombinedMeshAttributes.GetVertexInstanceUVs();
	const TVertexInstanceAttributesRef<FVector4>  TargetVertexInstanceColors        = CombinedMeshAttributes.GetVertexInstanceColors();
	//const  TVertexInstanceAttributesRef<FVector>   TargetVertexInstanceTangents      = CombinedMeshAttributes.GetVertexInstanceTangents();
	//const  TVertexInstanceAttributesRef<float>     TargetVertexInstanceBinormalSigns = CombinedMeshAttributes.GetVertexInstanceBinormalSigns();
		
	const int32 VertexCount = InMesh->Vertices.Num();
	TargetVertexInstanceUVs.SetNumIndices(InMesh->UVSetCount + 1); // We add 1 slot for Lightmap UV
	MeshDescription.ReserveNewVertices(VertexCount);
	MeshDescription.ReserveNewVertexInstances(VertexCount);
	MeshDescription.ReserveNewPolygons(InMesh->Header->FaceCount);
	MeshDescription.ReserveNewEdges(InMesh->Header->FaceCount * 2 );
	TMap<int32, FVertexID> VertexIndexToVertexID;
	VertexIndexToVertexID.Reserve(VertexCount);
	TMap<int32, FVertexInstanceID> VertexIndexToVertexInstanceID;
	VertexIndexToVertexInstanceID.Reserve(VertexCount);
	// Create Vertices
	for (int i = 0; i < VertexCount; i++)
	{
		const FVertexID VertexID = MeshDescription.CreateVertex();
		TargetVertexPositions[VertexID] = C2MUtilsImport::ConvertDir(InMesh->Vertices[i]);
		VertexIndexToVertexID.Add(i, VertexID);
		const FVertexInstanceID VertexInstanceID = MeshDescription.CreateVertexInstance(VertexID);
		VertexIndexToVertexInstanceID.Add(i, VertexInstanceID);
		TargetVertexInstanceNormals[VertexInstanceID] = C2MUtilsImport::ConvertDir(InMesh->Normals[i]);
		TargetVertexInstanceColors[VertexInstanceID] = InMesh->Colors[i].ToVector();
		//TargetVertexInstanceUVs.Set(VertexInstanceID, 0, UVs[i]);
		TArray<FVector2D> VertexUVs = InMesh->UVs[i];
		for (int u = 0; u < InMesh->UVSetCount; u++)
			TargetVertexInstanceUVs.Set(VertexInstanceID, u, FVector2D(VertexUVs[u].X, 1 - VertexUVs[u].Y)); // We gotta flip UVV
	}
	// Create polygons
	for (auto Surface : InMesh->Surfaces)
	{
		const FPolygonGroupID PolygonGroup = MeshDescription.CreatePolygonGroup();
		for (auto Face : Surface->Faces)
		{
			if (Face.index[0] == Face.index[1] ||
				Face.index[1] == Face.index[2] ||
				Face.index[2] == Face.index[0]) // Degenerate face
					continue;
			TArray<FVertexInstanceID> VertexInstanceIDs;
			VertexInstanceIDs.SetNum(3);
			// Grab Vertex Instances for current face
			for (int i = 0; i < 3; i++)
			{
				const uint32_t VertexIndex = Face.index[i];
				VertexInstanceIDs[i] = VertexIndexToVertexInstanceID[VertexIndex];
				
			}
			// Insert a polygon into the mesh
			MeshDescription.CreatePolygon(PolygonGroup, VertexInstanceIDs);
		}
		PolygonGroupNames[PolygonGroup] = FName(Surface->Name);
	}

	return MeshDescription;
}

void C2MStaticMesh::CreateStaticMeshFromMeshDescription(UC2MapHeader* MapInfo, FMeshDescription& inMeshDescription, C2Mesh* InMesh, UC2MImportOptions* ImportOptions, TArray<C2Material*> CoDMaterials)
{
	// Set Static Mesh Name & Path
	FString ObjectName = InMesh->Header->MeshName.Replace(TEXT("::"), TEXT("_"));
	FString pathPackage = FPaths::Combine(TEXT("/Game"),
										  TEXT("C2M"),
										  *MapInfo->GameVersion,
										  InMesh->bIsXModel ? TEXT("Models") : FPaths::Combine(TEXT("MapGeometry"), *MapInfo->MapName),
										  *ObjectName);
	
	FString PackageFileName = FPackageName::LongPackageNameToFilename(pathPackage, FPackageName::GetAssetPackageExtension());
	
	// Create Static Mesh
	UPackage* Package = CreatePackage(*pathPackage);
	check(Package);
	Package->FullyLoad();
	Package->Modify();
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, FName(*ObjectName), RF_Public | RF_Standalone);
	
	// Set default settings
	StaticMesh->InitResources();
	StaticMesh->SetLightingGuid();
	// StaticMesh->bGenerateMeshDistanceField = false;
	FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
	SrcModel.BuildSettings.bRecomputeNormals = false;
	SrcModel.BuildSettings.bRecomputeTangents = true;
	SrcModel.BuildSettings.bRemoveDegenerates = false;
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
	SrcModel.BuildSettings.bGenerateLightmapUVs = true;
	SrcModel.BuildSettings.SrcLightmapIndex = 0;
	SrcModel.BuildSettings.DstLightmapIndex = InMesh->UVSetCount; // We use last UV set for lightmap
	SrcModel.BuildSettings.bUseMikkTSpace = true;

	// Get & Set Mesh Description
	FMeshDescription* MeshDescription = StaticMesh->GetMeshDescription(0);
	
	if (!MeshDescription)
	{
		MeshDescription = StaticMesh->CreateMeshDescription(0);
	}
	
	*MeshDescription = inMeshDescription;
	StaticMesh->CommitMeshDescription(0);

	// Create materials and mesh sections
	for (int i = 0; i < InMesh->Surfaces.Num(); i++)
	{
		const auto Surface = InMesh->Surfaces[i];
		
		// Static Material for Surface
		FStaticMaterial&& UEMat = FStaticMaterial(UMaterial::GetDefaultMaterial(MD_Surface));
		if(ImportOptions->bImportMaterials)
		{
			// Create an array of Surface Materials
			TArray<C2Material*> SurfMaterials;
			SurfMaterials.Reserve(Surface->Materials.Num());
			for (const uint16_t MaterialIndex : Surface->Materials)
				SurfMaterials.Push(CoDMaterials[MaterialIndex]);
		
			UMaterialInterface* MaterialInstance = C2MMaterialInstance::CreateMixMaterialInstance(ImportOptions, SurfMaterials, MapInfo->GameVersion);
			UEMat = FStaticMaterial(MaterialInstance);
		}
		UEMat.UVChannelData.bInitialized = true;
		UEMat.MaterialSlotName = FName(Surface->Name);
		UEMat.ImportedMaterialSlotName = FName(Surface->Name);
		StaticMesh->GetStaticMaterials().Add(UEMat);
		StaticMesh->GetSectionInfoMap().Set(0, i, FMeshSectionInfo(i));
	}
	StaticMesh->GetOriginalSectionInfoMap().CopyFrom(StaticMesh->GetSectionInfoMap());
	StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
	// Editor builds cache the mesh description so that it can be preserved during map reloads etc
	
	TArray<FText> BuildErrors;
	// Build mesh from source
	StaticMesh->Build(false);
	StaticMesh->EnforceLightmapRestrictions();
	// StaticMesh->PostEditChange();
	StaticMesh->MarkPackageDirty();
	

	// Notify asset registry of new asset
	FAssetRegistryModule::AssetCreated(StaticMesh);
	ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(StaticMesh);
	UPackage::SavePackage(Package, StaticMesh, RF_Public | RF_Standalone, *PackageFileName, GError, nullptr, false, true, SAVE_NoError);
	
	UE_LOG(LogTemp, Log, TEXT("Static Mesh created: %s"), *ObjectName);
}

void C2MStaticMesh::CreateStaticMesh(UC2MapHeader* MapInfo, bool bSplitMesh, C2Mesh* InMesh, UC2MImportOptions* ImportOptions, TArray<C2Material*> CoDMaterials)
{
	// Do we split our mesh by surface? (Useful for Map Geometry)
    if (bSplitMesh)
    {
    	// Create slow task for sub meshes
    	FFormatNamedArguments Args;
    	Args.Add(TEXT("SubMeshIndex"), 1);
    	Args.Add(TEXT("ArrayLength"), InMesh->Header->SurfaceCount);
    	Args.Add(TEXT("SubMeshName"), FText::FromString(""));
    	FText SurfSlowTaskText = NSLOCTEXT("C2MFactory", "C2MCreatingSubMesh", "{SubMeshIndex} of {ArrayLength} - {SubMeshName}");
    	FScopedSlowTask SurfSlowTask(InMesh->Header->SurfaceCount, FText::Format(SurfSlowTaskText, Args));
    	SurfSlowTask.MakeDialog();
    	for (int i = 0; i < InMesh->Surfaces.Num(); i++)
    	{
	        const auto Surface = InMesh->Surfaces[i];
    		// Set sub mesh slow task
    		Args[TEXT("SubMeshIndex")] = i + 1;
    		Args[TEXT("SubMeshName")] = FText::FromString(Surface->Name);
    		SurfSlowTask.EnterProgressFrame(1, FText::Format(SurfSlowTaskText, Args));
    		// Create our base Sub Mesh
    		C2Mesh* SubMesh = new C2Mesh();
    		SubMesh->Header = new C2MeshHeader(Surface->Name);
    		SubMesh->MaterialCount = Surface->Materials.Num();
    		SubMesh->UVSetCount = Surface->UVCount;
    		// We need a single surface with faces & indices relative to the new Sub Mesh
    		C2MSurface* SubSurface = new C2MSurface(Surface);
    		SubMesh->Surfaces.Reserve(1);
    		// Since we're splitting our objects, we will need new indices for our verts
    		std::map<uint32_t, uint32_t> MainMeshIndexToSubMeshIndex;
    		uint32_t FaceVertexIndexCounter = 0;
    		for (auto& Face : Surface->Faces)
    		{
    			GfxFace NewFace;
    			// Loop through face vertices, and assign to our new face using new indices 
    			for (int v = 0; v < 3; v++)
    			{
    				uint32_t vertexIndex = Face.index[v];
    				// Make sure this vertex was not added already
    				if (MainMeshIndexToSubMeshIndex.find(vertexIndex) == MainMeshIndexToSubMeshIndex.end())
    				{
    					// Add vertex
    					SubMesh->Vertices.Add(InMesh->Vertices[vertexIndex]);
    					SubMesh->Normals.Add(InMesh->Normals[vertexIndex]);
    					SubMesh->Colors.Add(InMesh->Colors[vertexIndex]);
    					SubMesh->UVs.Add(InMesh->UVs[vertexIndex]);
    					SubMesh->Header->VertexCount++;

    					MainMeshIndexToSubMeshIndex[vertexIndex] = FaceVertexIndexCounter;
    					FaceVertexIndexCounter++;
    				}
    				// Assign new index to new face
    				NewFace.index[v] = MainMeshIndexToSubMeshIndex[vertexIndex];
    			}
    			SubMesh->Header->FaceCount++;
    			SubSurface->Faces.Add(NewFace);
    		}
    		SubMesh->Surfaces.Push(SubSurface);
    		// Create our Sub Mesh as Static Mesh
    		FMeshDescription SubMeshDescription = CreateMeshDescription(SubMesh);
    		CreateStaticMeshFromMeshDescription(MapInfo, SubMeshDescription, SubMesh, ImportOptions, CoDMaterials);
    	}
    }
    else
    {
    	FMeshDescription InMeshDescription = CreateMeshDescription(InMesh);
    	CreateStaticMeshFromMeshDescription(MapInfo, InMeshDescription, InMesh, ImportOptions, CoDMaterials);
    }
    	
}