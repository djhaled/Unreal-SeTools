#include "C2MStaticMesh.h"

#include "AssetToolsModule.h"
#include "C2MMaterialInstance.h"
#include "EditorModeManager.h"
#include "ObjectTools.h"
#include "StaticMeshAttributes.h"
#include "IMeshBuilderModule.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "MaterialDomain.h"
#include "PhysicsAssetUtils.h"
#include "Utils/C2MUtilsImport.h"
#include "Rendering/SkinWeightVertexBuffer.h"
#include "SkeletalMeshModelingTools/Private/SkeletalMeshModelingToolsMeshConverter.h"
#include "Rendering/SkeletalMeshLODImporterData.h"
#include "StaticMeshOperations.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkeletalMeshModel.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/SkinnedAssetCommon.h"

/* Create FRawMesh from C2Mesh
 * UNUSED
 */



UObject* C2MStaticMesh::CreateMesh(UObject* ParentPackage, FString ModelPackage,C2Mesh* InMesh, const TArray<C2Material*>& CoDMaterials)
{
	FMeshDescription InMeshDescription = CreateMeshDescription(InMesh);
	if (FPackageName::DoesPackageExist(ModelPackage)) { return nullptr; }
	UStaticMesh* StaticMesh = Cast<UStaticMesh>(CreateStaticMeshFromMeshDescription(ParentPackage,InMeshDescription,InMesh,CoDMaterials));
	if (MeshOptions->MeshType == EMeshType::SkeletalMesh)
	{
		USkeletalMesh* NewSkeletalMesh = Cast<USkeletalMesh>(CreateSkeletalMeshFromStaticMesh(StaticMesh,InMesh));

		StaticMesh->RemoveFromRoot();
		StaticMesh->MarkAsGarbage();

		return NewSkeletalMesh;
	}
	return StaticMesh;
}


UObject* C2MStaticMesh::CreateSkeletalMeshFromStaticMesh(UStaticMesh* Mesh, C2Mesh* InMesh)
{
	USkeleton* Skeleton = nullptr;
	USkeletalMesh* MeshSkel = nullptr;
	FReferenceSkeleton RefSkel;
	CreateSkeleton(InMesh,Mesh->GetName(),Mesh->GetPackage(),RefSkel,Skeleton);

	//
	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	USkeletalMeshFromStaticMeshFactory* SkeletalMeshFactory = NewObject<USkeletalMeshFromStaticMeshFactory>();
	SkeletalMeshFactory->StaticMesh = Cast<UStaticMesh>(Mesh);
	SkeletalMeshFactory->ReferenceSkeleton = RefSkel;
	SkeletalMeshFactory->Skeleton = Skeleton;
	MeshSkel = Cast<USkeletalMesh>(AssetTools.CreateAsset(Mesh->GetName(), FPackageName::GetLongPackagePath(Mesh->GetPackage()->GetName()), USkeletalMesh::StaticClass(), SkeletalMeshFactory));
	USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(MeshSkel);
	FMeshDescription DDesc;
	FSkeletalMeshImportData SkelMeshImportData;
	SkeletalMesh->LoadLODImportedData(0,SkelMeshImportData);
	TArray<SkeletalMeshImportData::FRawBoneInfluence> Influences;

	for (size_t i = 0; i < InMesh->Surfaces.Num(); i++)
	{
		for (size_t w = 0; w < InMesh->Surfaces[i]->Vertexes.Num(); w++)
		{
			auto weights = InMesh->Surfaces[i]->Vertexes[w].Weights;
			for (size_t d = 0; d < weights.Num(); d++)
			{
				auto weight = weights[d];
				if (weight.WeightValue > 0)
				{
					SkeletalMeshImportData::FRawBoneInfluence Influence;
					Influence.BoneIndex = weight.WeightID;
					Influence.VertexIndex = weight.VertexIndex;
					Influence.Weight = weight.WeightValue;
					Influences.Add(Influence);
				}
			}
		}
	}
	SkelMeshImportData.Influences = Influences;
	SkeletalMesh->SaveLODImportedData(0,SkelMeshImportData);
	SkeletalMesh->CalculateInvRefMatrices();
	FSkeletalMeshBuildSettings BuildOptions;
	BuildOptions.bRemoveDegenerates = true;
	BuildOptions.bRecomputeTangents = true;
	BuildOptions.bUseMikkTSpace = true;
	SkeletalMesh->GetLODInfo(0)->BuildSettings = BuildOptions;
	SkeletalMesh->SetImportedBounds(FBoxSphereBounds(FBoxSphereBounds3f(FBox3f(SkelMeshImportData.Points))));


	auto& MeshBuilderModule = IMeshBuilderModule::GetForRunningPlatform();
	const FSkeletalMeshBuildParameters SkeletalMeshBuildParameters(SkeletalMesh, GetTargetPlatformManagerRef().GetRunningTargetPlatform(), 0, false);
	if (!MeshBuilderModule.BuildSkeletalMesh(SkeletalMeshBuildParameters))
	{
		SkeletalMesh->BeginDestroy();
		return nullptr;
	}
	

	SkeletalMesh->PostEditChange();

	SkeletalMesh->SetSkeleton(Skeleton);
	if (!MeshOptions->OverrideSkeleton.IsValid())
	{
		Skeleton->MergeAllBonesToBoneTree(SkeletalMesh);
		Skeleton->SetPreviewMesh(SkeletalMesh);
	}
	FAssetRegistryModule::AssetCreated(SkeletalMesh);
	SkeletalMesh->MarkPackageDirty();

	Skeleton->PostEditChange();
	FAssetRegistryModule::AssetCreated(Skeleton);
	Skeleton->MarkPackageDirty();

	// Physics Asset
	FPhysAssetCreateParams NewBodyData;
	FString Phy_ObjectName = FString::Printf(TEXT("%s_PhysicsAsset"), *SkeletalMesh->GetName());
	auto PhysicsPackage = CreatePackage(*FPaths::Combine(FPaths::GetPath(Mesh->GetPackage()->GetPathName()), Phy_ObjectName));
	UPhysicsAsset* PhysicsAsset = NewObject<UPhysicsAsset>(PhysicsPackage, FName(*Phy_ObjectName), RF_Public | RF_Standalone);
	FText CreationErrorMessage;
	FPhysicsAssetUtils::CreateFromSkeletalMesh(PhysicsAsset, SkeletalMesh, NewBodyData, CreationErrorMessage);
	if (!SkeletalMesh->GetPhysicsAsset())
	{
		SkeletalMesh->SetPhysicsAsset(PhysicsAsset);
	}
	PhysicsAsset->MarkPackageDirty();
	PhysicsAsset->PostEditChange();
	FAssetRegistryModule::AssetCreated(PhysicsAsset);

	return SkeletalMesh;
	
}
FMeshDescription C2MStaticMesh::CreateMeshDescription(C2Mesh* InMesh)
{
    // Prepare our base Mesh Description
    FMeshDescription MeshDescription;
    FStaticMeshAttributes CombinedMeshAttributes{ MeshDescription };
    CombinedMeshAttributes.Register();
    CombinedMeshAttributes.RegisterTriangleNormalAndTangentAttributes();

    const TVertexAttributesRef<FVector3f> TargetVertexPositions = CombinedMeshAttributes.GetVertexPositions();
    const TPolygonGroupAttributesRef<FName> PolygonGroupNames = CombinedMeshAttributes.GetPolygonGroupMaterialSlotNames();
    const TVertexInstanceAttributesRef<FVector3f> TargetVertexInstanceNormals = CombinedMeshAttributes.GetVertexInstanceNormals();
    const TVertexInstanceAttributesRef<FVector2f> TargetVertexInstanceUVs = CombinedMeshAttributes.GetVertexInstanceUVs();
    const TVertexInstanceAttributesRef<FVector4f> TargetVertexInstanceColors = CombinedMeshAttributes.GetVertexInstanceColors();

    const int32 VertexCount = InMesh->Header->BoneCountBuffer;
    TargetVertexInstanceUVs.SetNumChannels(InMesh->UVSetCount + 1); // We add 1 slot for Lightmap UV
    MeshDescription.ReserveNewVertices(InMesh->Header->BoneCountBuffer);
    MeshDescription.ReserveNewVertexInstances(InMesh->Header->BoneCountBuffer);
    MeshDescription.ReserveNewPolygons(InMesh->Header->MaterialCountBuffer);
    MeshDescription.ReserveNewEdges(InMesh->Header->MaterialCountBuffer * 2);
    TArray<FVertexID> VertexIndexToVertexID;
    VertexIndexToVertexID.Reserve(VertexCount);
    TArray<FVertexInstanceID> VertexIndexToVertexInstanceID;
    VertexIndexToVertexInstanceID.Reserve(VertexCount);
    int32 GlobalVertexIndex = 0;
	TArray<FVector3f> Verts;

    // Create vertices and vertex instances
    for (auto surfirst : InMesh->Surfaces)
    {
        for (int i = 0; i < surfirst->Vertexes.Num(); i++)
        {
            const FVertexID VertexID = MeshDescription.CreateVertex();
        	TargetVertexPositions[VertexID] = FVector3f(surfirst->Vertexes[i].Vertice.X, -surfirst->Vertexes[i].Vertice.Y,
											surfirst->Vertexes[i].Vertice.Z);
            VertexIndexToVertexID.Add(VertexID);
        	
            const FVertexInstanceID VertexInstanceID = MeshDescription.CreateVertexInstance(VertexID);
            VertexIndexToVertexInstanceID.Add(VertexInstanceID);
            GlobalVertexIndex++;

        	TargetVertexInstanceNormals[VertexInstanceID] =  FVector3f(surfirst->Vertexes[i].Normal.X, -surfirst->Vertexes[i].Normal.Y,
											surfirst->Vertexes[i].Normal.Z);
        	
            TargetVertexInstanceColors[VertexInstanceID] = surfirst->Vertexes[i].Color.ToVector();

            //TArray<FVector2f> VertexUVs = InMesh->UVs[i];
            for (int u = 0; u < InMesh->UVSetCount; u++)
            {
				//TargetVertexInstanceUVs.Set(VertexInstanceID, u, FVector2f(surfirst->Vertexes[i].UV.X, 1 - surfirst->Vertexes[i].UV.Y));
                TargetVertexInstanceUVs.Set(VertexInstanceID, u, FVector2f(surfirst->Vertexes[i].UV.X, surfirst->Vertexes[i].UV.Y)); // We gotta flip UV
            }
        }
    }

    // Create polygons and assign polygon group names
    for (auto Surface : InMesh->Surfaces)
    {
        const FPolygonGroupID PolygonGroup = MeshDescription.CreatePolygonGroup();

        for (int f = 0; f < Surface->Faces.Num(); f++)
        {
            auto Face = Surface->Faces[f];

            // Check for degenerate face (where two or more indices are the same)
            if (Face.index[0] == Face.index[1] ||
                Face.index[1] == Face.index[2] ||
                Face.index[2] == Face.index[0])
            {
                // Skip processing degenerate faces
                continue;
            }

            TArray<FVertexInstanceID> VertexInstanceIDs;
            VertexInstanceIDs.SetNum(3);
			// Rearrange vertex indices to match Blender order


            // Grab Vertex Instances for the current face

        	for (int i = 0; i < 3; i++)
            {
            	const uint32_t VertexIndex = Face.index[i];
            	VertexInstanceIDs[i] = VertexIndexToVertexInstanceID[VertexIndex];

            }


            MeshDescription.CreatePolygon(PolygonGroup, VertexInstanceIDs);
        }

        PolygonGroupNames[PolygonGroup] = FName(Surface->Name);
    }

    return MeshDescription;
}


UObject* C2MStaticMesh::CreateStaticMeshFromMeshDescription(UObject* ParentPackage,FMeshDescription& inMeshDescription, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials)
{
	FString ObjectName = InMesh->Header->MeshName.Replace(TEXT("::"), TEXT("_"));
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(ParentPackage, FName(*ObjectName), RF_Public | RF_Standalone);
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
	TArray<FStaticMaterial> StaticMaterials;
	// Create materials and mesh sections
	for (int i = 0; i < InMesh->Surfaces.Num(); i++)
	{
		const auto Surface = InMesh->Surfaces[i];
		
		// Static Material for Surface
		FStaticMaterial&& UEMat = FStaticMaterial(UMaterial::GetDefaultMaterial(MD_Surface));
		if(Surface->Materials.Num() > 0 && MeshOptions->bImportMaterials)
		{
			// Create an array of Surface Materials
			TArray<C2Material*> SurfMaterials;
			SurfMaterials.Reserve(Surface->Materials.Num());
			for (const uint16_t MaterialIndex : Surface->Materials)
				SurfMaterials.Push(CoDMaterials[MaterialIndex]);
			UMaterialInterface* MaterialInstance = C2MMaterialInstance::CreateMixMaterialInstance( SurfMaterials,ParentPackage,MeshOptions->OverrideMasterMaterial.LoadSynchronous());
			UEMat = FStaticMaterial(MaterialInstance);
		}
		UEMat.UVChannelData.bInitialized = true;
		UEMat.MaterialSlotName = FName(Surface->Name);
		UEMat.ImportedMaterialSlotName = FName(Surface->Name);
		StaticMaterials.Add(UEMat);
		StaticMesh->GetSectionInfoMap().Set(0, i, FMeshSectionInfo(i));
	}
	StaticMesh->SetStaticMaterials(StaticMaterials);
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
	return StaticMesh;
}




void C2MStaticMesh::ProcessSkeleton(const FSkeletalMeshImportData& ImportData, const USkeleton* Skeleton, FReferenceSkeleton& OutRefSkeleton, int& OutSkeletalDepth)
{
	const auto RefBonesBinary = ImportData.RefBonesBinary;
	OutRefSkeleton.Empty();

	FReferenceSkeletonModifier RefSkeletonModifier(OutRefSkeleton, Skeleton);

	for (const auto Bone : RefBonesBinary)
	{
		const FMeshBoneInfo BoneInfo(FName(*Bone.Name), Bone.Name, Bone.ParentIndex);
		RefSkeletonModifier.Add(BoneInfo, FTransform(Bone.BonePos.Transform));
	}

	OutSkeletalDepth = 0;

	TArray<int> SkeletalDepths;
	SkeletalDepths.Empty(ImportData.RefBonesBinary.Num());
	SkeletalDepths.AddZeroed(ImportData.RefBonesBinary.Num());
	for (auto b = 0; b < OutRefSkeleton.GetNum(); b++)
	{
		const auto Parent = OutRefSkeleton.GetParentIndex(b);
		auto Depth = 1.0f;

		SkeletalDepths[b] = 1.0f;
		if (Parent != INDEX_NONE)
		{
			Depth += SkeletalDepths[Parent];
		}
		if (OutSkeletalDepth < Depth)
		{
			OutSkeletalDepth = Depth;
		}
		SkeletalDepths[b] = Depth;
	}
}


void C2MStaticMesh::CreateSkeleton(C2Mesh* InMesh, FString ObjectName, UPackage* ParentPackage, FReferenceSkeleton& OutRefSkeleton, USkeleton*& OutSkeleton)
{
	FSkeletalMeshImportData SkelMeshImportData;
	
	for (auto PskBone : InMesh->Bones)
	{
		SkeletalMeshImportData::FBone Bone;
		Bone.Name = PskBone.Name;
		Bone.ParentIndex = PskBone.ParentIndex == -1 ? INDEX_NONE : PskBone.ParentIndex;
		FVector3f PskBonePos = PskBone.LocalPosition;
		auto PskBoneRotEu = PskBone.LocalRotation;
		PskBoneRotEu.Yaw *= -1.0;
		PskBoneRotEu.Roll *= -1.0;
		if (Bone.Name == "j_mainroot")
		{
			PskBoneRotEu.Roll = MeshOptions->OverrideSkeletonRootRoll;
		}
		FQuat4f PskBoneRot = PskBoneRotEu.Quaternion();
		FTransform3f PskTransform;

		PskTransform.SetLocation(FVector4f(PskBonePos.X,-PskBonePos.Y,PskBonePos.Z));
		PskTransform.SetRotation(PskBoneRot);
		SkeletalMeshImportData::FJointPos BonePos;
		BonePos.Transform = PskTransform;
		Bone.BonePos = BonePos;
		SkelMeshImportData.RefBonesBinary.Add(Bone);
	}
	auto newName = "SK_" + ObjectName;
	auto SkeletonPackage = CreatePackage(*FPaths::Combine(FPaths::GetPath(ParentPackage->GetPathName()), newName));
	OutSkeleton = MeshOptions->OverrideSkeleton.IsValid() ? MeshOptions->OverrideSkeleton.LoadSynchronous() : NewObject<USkeleton>(SkeletonPackage, FName(*newName), RF_Public | RF_Standalone);
	auto SkeletalDepth = 0;
	ProcessSkeleton(SkelMeshImportData, OutSkeleton, OutRefSkeleton, SkeletalDepth);
}



