#pragma once
#include "Structures/C2Material.h"
#include "MeshDescription.h"
#include "Structures/C2Mesh.h"
#include "Widgets/Meshes/UserMeshOptions.h"




class C2MStaticMesh
{
public:
	UUserMeshOptions* MeshOptions;
	UObject* CreateMesh(UObject* ParentPackage,FString ModelPackage, C2Mesh* InMesh,  const TArray<C2Material*>& CoDMaterials);
	FMeshDescription CreateMeshDescription(C2Mesh* InMesh);
	UObject* CreateStaticMeshFromMeshDescription(UObject* ParentPackage,FMeshDescription& inMeshDescription, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials);
	void ProcessSkeleton(const FSkeletalMeshImportData& ImportData, const USkeleton* Skeleton, FReferenceSkeleton& OutRefSkeleton, int& OutSkeletalDepth);
	UObject* CreateSkeletalMeshFromMeshDescription( UObject* ParentPackage,FMeshDescription& InMeshDescription, C2Mesh* InMesh,  TArray<C2Material*> CoDMaterials);
	void CreateSkeleton(C2Mesh* InMesh, FString ObjectName, UPackage* ParentPackage, FReferenceSkeleton& OutRefSkeleton, USkeleton*& OutSkeleton);

	UObject* CreateSkeletalMeshFromStaticMesh(UStaticMesh* Mesh, C2Mesh* InMesh);
};

